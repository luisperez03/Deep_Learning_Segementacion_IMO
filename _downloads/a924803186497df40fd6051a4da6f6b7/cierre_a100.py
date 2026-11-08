"""Reconstruye el benchmark de cinco redes desde los dos ZIP de resultados."""
from pathlib import Path
from zipfile import ZipFile
import hashlib
import json
import tarfile
import numpy as np
import pandas as pd
from scipy.stats import ttest_rel
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parents[1]
BOOK = ROOT / "libro"
OUT = ROOT / "resultados" / "cierre_v2"
OUT.mkdir(exist_ok=True)
ARCHIVE = ROOT / "colab/IMO_resultados_A100.zip"
SEEDS = [11, 23, 37, 53, 71]
NAMES = {"fcn": "FCN", "unet": "U-Net", "segnet": "SegNet",
         "deeplab_compacta": "DeepLab compacta", "pspnet_compacta": "PSPNet compacta"}
NEW = list(NAMES)[-2:]
METRICS = ["Dice", "IoU", "mIoU", "accuracy"]
PAIRS = ["model", "seed"]
MANIFEST = ROOT / "resultados/preentrenamiento/test.csv"
test = pd.read_csv(MANIFEST).set_index("id").group_id.sort_index()
assert len(test) == 218 and test.index.is_unique and test.nunique() == 122

def fmt(x):
    return f"{x:.4f}".replace(".", ",")

def table(headers, rows):
    return "\n".join(["| " + " | ".join(headers) + " |", "|" + "---|" * len(headers)] +
                     ["| " + " | ".join(map(str, row)) + " |" for row in rows])

with ZipFile(ROOT / "IMO_test_final.zip") as z:
    old = pd.read_csv(z.open("por_imagen.csv"))
with tarfile.open(ROOT / "piloto_T4_final.tar.gz") as t:
    records = json.load(t.extractfile("IMO_colab_resultados_v1/continuacion_cpu_gpu/resumen.json"))
histories = []
sha = hashlib.sha256(ARCHIVE.read_bytes()).hexdigest()
with ZipFile(ARCHIVE) as z:
    assert z.testzip() is None
    new = pd.read_csv(z.open("por_imagen.csv"))
    exported = pd.read_csv(z.open("resumen_test.csv")).set_index(["modelo","semilla"])
    json_summary = pd.DataFrame(json.loads(z.read("resumen_test.json"))).set_index(["modelo","semilla"])
    pd.testing.assert_frame_equal(exported.sort_index(), json_summary.sort_index(), check_exact=False)
    assert len(exported) == 10 and exported.index.is_unique
    for m in NEW:
        for s in SEEDS:
            folder = f"{m}_seed{s}_lr0.001/"
            cfg, r, h = [json.loads(z.read(folder+k+".json")) for k in ["config","result","history"]]
            assert cfg["model"] == m and cfg["seed"] == s and r["completed"]
            assert [cfg[k] for k in ["lr","epochs","patience","batch_size","size"]] == [.001,30,5,1,256]
            assert len(h) == r["epochs_completed"] and [v["epoch"] for v in h] == list(range(1,len(h)+1))
            assert np.isclose(max(v["val_group_Dice"] for v in h),r["best_val_group_Dice"])
            for f in ["last.pt","best.pt","best_val_predictions.json"]:
                assert z.getinfo(folder+f).file_size > 0
            histories.extend(dict(model=m,seed=s,**v) for v in h)
            records.append(r)
data = pd.concat([old,new],ignore_index=True)
assert set(data.model)==set(NAMES) and set(data.seed)==set(SEEDS)
assert len(data)==5450 and not data.duplicated(PAIRS+["id"]).any()
assert data[METRICS].apply(lambda col: col.between(0,1).all()).all()
for _,d in data.groupby(PAIRS):
    assert d.set_index("id").group_id.sort_index().equals(test)
groups = data.groupby(PAIRS+["group_id"])[METRICS].mean()
runs = groups.groupby(PAIRS).mean()
for (m,s),r in exported.iterrows():
    assert np.isclose(r.Dice_test_por_grupo,runs.loc[(m,s),"Dice"]) and (r.n_imagenes_test,r.n_grupos_test)==(218,122)
pilot = pd.DataFrame(records).set_index(PAIRS)
assert len(pilot)==25 and pilot.index.is_unique and set(pilot.index)==set(runs.index)
runs = runs.join(pilot[["parameters","epochs_completed","best_val_group_Dice"]])
runs.to_csv(OUT/"metricas_25_corridas.csv")
draws=np.random.default_rng(20261108).integers(122,size=(10000,122))
def ci(x):
    return np.quantile(np.asarray(x)[draws].mean(axis=1),[.025,.975])
summary=[]
for m in NAMES:
    fixed=groups.loc[m].groupby("group_id").mean()
    for k in METRICS:
        x=fixed[k];lo,hi=ci(x)
        summary.append([m,k,x.mean(),runs.loc[m,k].std(),lo,hi])
bench=pd.DataFrame(summary,columns=["model","metric","mean","sd_seed","lower","upper"])
bench.to_csv(OUT/"benchmark.csv",index=False)
rank=runs.groupby("model").mean().sort_values("Dice",ascending=False)
rows=[]
for m,r in rank.iterrows():
    interval=bench[(bench.model==m)&(bench.metric=="Dice")].iloc[0]
    rows.append([NAMES[m],fmt(r.best_val_group_Dice),fmt(r.Dice),fmt(interval.sd_seed),
                 f"{fmt(interval.lower)} a {fmt(interval.upper)}",fmt(r.IoU),fmt(r.mIoU),int(r.parameters)])
main_table=table(["Red","Dice val.","Dice test","DE semillas","IC 95 % Dice","IoU","mIoU","Parámetros"],rows)
blocks=groups.Dice.groupby(["group_id","model"]).mean().unstack()
contrasts=[]
for m in NEW:
    v=blocks[m]-blocks.fcn
    lo,hi=ci(v)
    contrasts.append([m,v.mean(),lo,hi,ttest_rel(blocks[m],blocks.fcn).pvalue])
adj=sorted(range(2),key=lambda i:contrasts[i][-1])
p0,p1=[contrasts[i][-1] for i in adj]
for i,p in zip(adj,[min(1,2*p0),min(1,max(2*p0,p1))]): contrasts[i].append(p)
contrasts_table=table(["Modelo menos FCN","Δ Dice","IC 95 % pareado","p t pareado","p Holm"],
    [[NAMES[m],fmt(d),f"{fmt(lo)} a {fmt(hi)}",fmt(p),fmt(ph)] for m,d,lo,hi,p,ph in contrasts])
hist=pd.DataFrame(histories)
fig,axes=plt.subplots(2,2,figsize=(9,5),sharex=True)
for j,m in enumerate(NEW):
    for s,h in hist[hist.model==m].groupby("seed"):
        for i,k in enumerate(["train_loss","val_group_Dice"]):
            axes[i,j].plot(h.epoch,h[k],label=str(s),lw=1.3)
    axes[0,j].set_title(NAMES[m]);axes[1,j].set_xlabel("Época")
axes[0,0].set_ylabel("Pérdida de entrenamiento");axes[1,0].set_ylabel("Dice de validación")
axes[0,1].legend(title="Semilla",fontsize=7,ncol=2)
fig.tight_layout();fig.savefig(BOOK/"figuras/curvas_a100.png",dpi=145);plt.close(fig)
inv=pd.read_csv(ROOT/"resultados/inicio_voc/inventario.csv")
d=data.merge(inv[["id","foreground_fraction"]],on="id",validate="many_to_one")
d["area"]=pd.cut(d.foreground_fraction,[-1,.02,.05,1],labels=["≤2 %","2–5 %",">5 %"])
area=d.groupby(["model","seed","area","group_id"],observed=True).Dice.mean().groupby(["model","area"],observed=True).mean().unstack()
sizes=table(["Modelo",*area.columns],[[NAMES[m],*[fmt(x) for x in area.loc[m]]] for m in rank.index])
bestval=rank.best_val_group_Dice.idxmax()
conclusion=(f"{NAMES[bestval]} obtuvo el mayor Dice medio de validación ({fmt(rank.loc[bestval,'best_val_group_Dice'])}). "
    f"En test, PSPNet compacta encabeza las cinco redes ({fmt(rank.loc['pspnet_compacta','Dice'])}), seguida de DeepLab compacta "
    f"({fmt(rank.loc['deeplab_compacta','Dice'])}) y FCN ({fmt(rank.loc['fcn','Dice'])}). "
    "Las dos mejoras medias frente a FCN quedan por debajo del margen práctico de 0,02. "
    "Los intervalos pareados permiten valorar la incertidumbre alrededor de esas mejoras.")
report=["## Comparación final de cinco líneas base",conclusion,main_table,
    "Cada imagen se pondera dentro de su grupo y cada grupo recibe el mismo peso. El Dice de validación corresponde al mejor checkpoint de cada semilla. La DE describe la variación entre cinco entrenamientos. Los IC percentiles de test se calcularon con 10.000 remuestreos de los 122 grupos y semillas fijas.",
    "### Contrastes previstos frente a FCN",contrasts_table,
    "Las pruebas t bilaterales contrastan la diferencia media entre grupos, después de promediar las cinco semillas. Holm ajusta los dos valores p. Los IC son individuales. La inferencia supone independencia entre grupos y se restringe a esta partición y a estos entrenamientos.",
    "### Tamaño del rótulo y errores",sizes,
    "Los estratos se definen por la fracción anotada de cada imagen. Un grupo puede estar representado en más de un estrato. Los objetos pequeños son los más difíciles para las cinco redes. En condiciones portuarias, este patrón importa porque los rótulos distantes ocupan menos píxeles.",
    "### Curvas de las 25 corridas","![Pérdida y Dice por época de las cinco redes](figuras/curvas_cinco.png)",
    "Se eligió el primer máximo de Dice por grupo en validación. Las curvas terminan al alcanzar 30 épocas o al acumular cinco épocas sin mejora. La pérdida de entrenamiento y el Dice de validación tienen escalas distintas.",
    "### Coste observado y alcance","Tiempo medio registrado por corrida en A100, incluyendo entrenamiento y validación: "+
    ", ".join(f"{NAMES[m]}, {hist[hist.model==m].groupby('seed').seconds.sum().mean()/60:.1f} min" for m in NEW)+
    ". FCN, SegNet y U-Net se entrenaron en CPU y T4. Las diferencias de duración mezclan arquitectura, hardware, caché y perfil numérico. El conteo de parámetros sí se puede comparar directamente.",
    "FCN conserva su papel de referencia del benchmark. PSPNet compacta ofrece el mayor Dice observado en test y su mejora media queda por debajo de δ. La validación sobre otros puertos y otras particiones sigue abierta. El diseño aporta cinco semillas por arquitectura, una partición agrupada y configuraciones fijas. No se realizó búsqueda de hiperparámetros, validación cruzada repetida ni una segunda anotación independiente.",
    "Los datos de las tablas están disponibles como {download}`métricas de las 25 corridas <../resultados/cierre_v2/metricas_25_corridas.csv>` y {download}`resumen con intervalos de las cuatro métricas <../resultados/cierre_v2/benchmark.csv>`. El {download}`script de cierre <../scripts/cierre_a100.py>` comprueba identificadores, grupos, semillas y concordancia con los resúmenes del ZIP."]
(BOOK/"cierre_a100.inc").write_text("\n\n".join(report)+"\n",encoding="utf8")
(ROOT/"docs/RESUMEN_EJECUTIVO.md").write_text("\n\n".join(["# Resumen ejecutivo del Entregable 1 · versión 2",
    "Se compararon cinco arquitecturas de segmentación binaria en HAZMAT-13, entrenadas desde cero con las mismas cinco semillas, Adam a 0,001 y un máximo de 30 épocas. El test contiene 218 imágenes en 122 grupos reservados. Se verificaron las 5.450 evaluaciones imagen–modelo–semilla sobre el mismo manifiesto.",main_table,conclusion,
    "La menor ocupación del rótulo se asocia a menor Dice en los cinco modelos. La revisión sistemática aporta 96 estudios, con tareas y métricas heterogéneas. La justificación de las cinco redes se documenta en Literatura. El efecto del hardware y del perfil numérico limita la comparación de tiempos entre fases."])+"\n",encoding="utf8")
print("SHA-256 del ZIP A100:",sha)
