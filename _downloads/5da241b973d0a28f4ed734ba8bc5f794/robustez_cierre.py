"""Auditoría local de inferencia de 25 checkpoints sobre grupos del test congelado."""
from pathlib import Path
from zipfile import ZipFile
import csv, hashlib, io, json, tarfile, time
import numpy as np
from PIL import Image, ImageFilter
import torch
from modelos_base import MODELS
from bases_v2 import BASES
from preparar_base import letterbox
from dataset_ready import metrics

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/"resultados/cierre_v2"
seedset=[11,23,37,53,71]
models={**MODELS,**BASES}
torch.set_num_threads(2)
torch.manual_seed(20261108)
rng=np.random.default_rng(20261108)
def tensor(image,norm):
    x,valid=letterbox(image.convert("RGB"),norm["size"])
    a=(np.asarray(x,dtype=np.float32)/255-np.array(norm["mean"],np.float32))/np.array(norm["std"],np.float32)
    a[~valid]=0
    return torch.from_numpy(a.transpose(2,0,1).copy())[None],valid
def save_csv(name,rows):
    with (OUT/name).open("w",newline="",encoding="utf8") as f:
        w=csv.DictWriter(f,fieldnames=rows[0]);w.writeheader();w.writerows(rows)
prep=ROOT/"resultados/preentrenamiento"
norm=json.loads((prep/"normalization_train.json").read_text())
with (prep/"test.csv").open(encoding="utf-8-sig") as f: rows=list(csv.DictReader(f))
groups=sorted(set(r["group_id"] for r in rows))
sample=[]
with ZipFile(ROOT/"colab/test_v2.zip") as z:
    for g in sorted(rng.choice(groups,24,replace=False)):
        r=rng.choice([r for r in rows if r["group_id"]==g])
        raw=z.read(r["image_path"]);assert hashlib.sha256(raw).hexdigest()==r["image_sha256"]
        image=Image.open(io.BytesIO(raw)).convert("RGB")
        raw=z.read("tesis_imo_para_codex/"+r["binary_mask_path"])
        assert hashlib.sha256(raw).hexdigest()==r["binary_mask_sha256"]
        mask,_=letterbox(Image.open(io.BytesIO(raw)).convert("L"),norm["size"],True)
        y=np.asarray(mask,dtype=bool)
        a=np.asarray(image,dtype=np.float32)
        variants={"original":image,"brillo":Image.fromarray(np.clip(a*1.25,0,255).astype("uint8")),
                  "ruido":Image.fromarray(np.clip(a+rng.normal(0,12,a.shape),0,255).astype("uint8")),
                  "desenfoque":image.filter(ImageFilter.GaussianBlur(2))}
        sample.append((r["id"],g,y,[(k,*tensor(im,norm)) for k,im in variants.items()]))
scores=[];cost=[]
with torch.inference_mode(), ZipFile(ROOT/"colab/IMO_resultados_A100.zip") as z, tarfile.open(ROOT/"piloto_T4_final.tar.gz") as t:
    for m,constructor in models.items():
        times=[]
        for s in seedset:
            folder=f"{m}_seed{s}_lr0.001/best.pt"
            if m in BASES: raw=z.read(folder)
            elif s==11 or (m=="fcn" and s==23): raw=(ROOT/"resultados/piloto_local_v1"/folder).read_bytes()
            else: raw=t.extractfile("IMO_colab_resultados_v1/continuacion_cpu_gpu/"+folder).read()
            net=constructor().eval()
            net.load_state_dict(torch.load(io.BytesIO(raw),map_location="cpu",weights_only=True))
            mac=[0]
            def count(layer,args,output):
                mac[0]+=output.numel()*layer.weight[0].numel()
            hooks=[layer.register_forward_hook(count) for layer in net.modules() if isinstance(layer,torch.nn.Conv2d)]
            net(sample[0][3][0][1])
            for handle in hooks: handle.remove()
            for uid,g,y,variants in sample:
                for k,x,valid in variants:
                    start=time.perf_counter();logits=net(x);ms=(time.perf_counter()-start)*1000
                    if k=="original": times.append(ms)
                    p=logits[0,0].sigmoid().numpy()
                    d=metrics(p,y,valid)
                    scores.append(dict(model=m,seed=s,id=uid,group_id=g,condition=k,Dice=d["Dice"],
                        fp=int(((p>=.5)&~y&valid).sum()),fn=int(((p<.5)&y&valid).sum())))
        cost.append(dict(model=m,ms_mediana=np.median(times),ms_p90=np.percentile(times,90),GMAC_conv=mac[0]/1e9))
save_csv("robustez_24_grupos.csv",scores);save_csv("inferencia_cpu.csv",cost)
