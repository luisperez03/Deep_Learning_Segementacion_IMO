"""Figuras de arquitectura, diseño y curvas de las cinco redes del benchmark."""
from pathlib import Path
import json
import tarfile
from zipfile import ZipFile
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch

root=Path(__file__).resolve().parents[1]
out=root/"libro/figuras"
names={"fcn":"FCN","segnet":"SegNet","unet":"U-Net","deeplab_compacta":"DeepLab compacta","pspnet_compacta":"PSPNet compacta"}
colors=["#315b87","#36766f","#83538c","#b7662b","#4e6d34"]
def diagram(file,title,subtitle,rows):
    fig,ax=plt.subplots(figsize=(12,1.5+len(rows)*.95))
    ax.set_xlim(0,12);ax.set_ylim(0,len(rows));ax.axis("off")
    fig.suptitle(title,x=.04,ha="left",fontsize=17,weight="bold",color="#193a48")
    fig.text(.04,.88,subtitle,fontsize=10,color="#354d55")
    for i,(label,body,end) in enumerate(rows):
        y=len(rows)-i-.5;c=colors[i%5]
        for x,w,t,fc,tc in [(0,2.15,label,c,"white"),(2.45,6.1,body,"#f1f5f6","#203a43"),(8.9,3.1,end,"#e5edf0","#203a43")]:
            ax.add_patch(FancyBboxPatch((x+.05,y-.32),w-.1,.64,boxstyle="round,pad=0.035",facecolor=fc,linewidth=0))
            ax.text(x+w/2,y,t,va="center",ha="center",fontsize=10,color=tc,weight="bold" if x==0 else "normal")
        for x in [2.15,8.55]:
            ax.annotate("",(x+.29,y),(x,y),arrowprops={"arrowstyle":"->","color":"#67828c"})
    fig.subplots_adjust(left=.04,right=.97,bottom=.04,top=.82)
    fig.savefig(out/file,dpi=155,bbox_inches="tight");plt.close(fig)
diagram("arquitecturas_cinco.png","Cinco caminos de una fotografía a su máscara",
    "Las cinco redes codifican la entrada RGB en tres niveles (16, 32 y 64 canales). Cada salida asigna un logit a cada píxel.",
    [("FCN","Mapa profundo → cabeza de 1 × 1","Interpolación bilineal"),
     ("SegNet","Índices de máximos → dos etapas de unpooling","Cabeza a 256 × 256"),
     ("U-Net","Dos ampliaciones → concatenación con mapas del encoder","Cabeza a 256 × 256"),
     ("DeepLab compacta","Ramas 1 × 1, dilataciones 2/4/8 y promedio global","Fusión → interpolación"),
     ("PSPNet compacta","Promedios en rejillas 1/2/3/6 + mapa profundo","Fusión → interpolación")])
diagram("flujo_cinco.png","Diseño del experimento de segmentación",
    "Los grupos de similitud se asignan completos a una partición. Cada red se entrena desde cero con las mismas cinco semillas.",
    [("Entrenamiento","1.015 imágenes · FCN, SegNet, U-Net, DeepLab y PSPNet","5 redes × 5 semillas"),
     ("Validación","218 imágenes · Dice medio por grupo en cada época","Mejor época por corrida"),
     ("Test interno","218 imágenes · 122 grupos · umbral fijo de 0,5","25 modelos evaluados")])
diagram("mapa_deeplab_compacta.png","DeepLab compacta · contexto por dilatación","La cabeza opera sobre un mapa profundo de 64 canales y 64 × 64 posiciones.",[
    ("Ramas locales","Una convolución 1 × 1 y tres de 3 × 3 dilatadas (2, 4, 8)","4 mapas × 16 canales"),
    ("Rama global","Promedio espacial → convolución 1 × 1 → ampliar a 64 × 64","1 mapa × 16 canales"),
    ("Reconstrucción","Concatenar 80 canales → 32 canales → 1 logit","Interpolar a 256 × 256")])
diagram("mapa_pspnet_compacta.png","PSPNet compacta · contexto por regiones","Cada rama resume una rejilla distinta del mismo mapa profundo.",[
    ("Pirámide","Promedio adaptativo en rejillas de 1, 2, 3 y 6 por lado","4 mapas × 16 canales"),
    ("Fusión","Ampliar las ramas y concatenarlas con el mapa de 64 canales","128 canales a 64 × 64"),
    ("Reconstrucción","Cabeza 1 × 1 → 32 canales → 1 logit","Interpolar a 256 × 256")])
fig,axs=plt.subplots(2,5,figsize=(14,5),sharex=True,sharey="row")
with tarfile.open(root/"piloto_T4_final.tar.gz") as t, ZipFile(root/"colab/IMO_resultados_A100.zip") as z:
    prefix="IMO_colab_resultados_v1/continuacion_cpu_gpu/"
    for j,(name,label) in enumerate(names.items()):
        for seed in (11,23,37,53,71):
            path=f"{name}_seed{seed}_lr0.001/history.json"
            if j>=3: h=json.loads(z.read(path))
            elif seed==11 or (name=="fcn" and seed==23):h=json.loads((root/"resultados/piloto_local_v1"/path).read_text())
            else:h=json.load(t.extractfile(prefix+path))
            for i,key in enumerate(["train_loss","val_group_Dice"]):
                axs[i,j].plot([r["epoch"] for r in h],[r[key] for r in h],lw=1.25,label=str(seed))
        axs[0,j].set_title(label,fontsize=11);axs[1,j].set_xlabel("Época")
axs[0,0].set_ylabel("Pérdida en train");axs[1,0].set_ylabel("Dice en validación")
fig.legend(*axs[0,0].get_legend_handles_labels(),loc="lower center",ncol=5,title="Semilla")
fig.tight_layout(rect=(0,.14,1,1));fig.savefig(out/"curvas_cinco.png",dpi=145);plt.close(fig)
