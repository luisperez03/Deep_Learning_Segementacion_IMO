## Ampliación del benchmark histórico

Promedios: primero imágenes dentro del grupo, luego grupos con igual peso. DE: cinco semillas. IC 95 % percentil: 10.000 remuestreos de los 122 grupos manteniendo las semillas fijas; no estima variabilidad entre particiones.

| Red | Métrica | Media | DE semillas | IC inf. | IC sup. |
|---|---|---|---|---|---|
| fcn | Dice | 0.8531 | 0.0063 | 0.8284 | 0.8761 |
| fcn | IoU | 0.7673 | 0.0077 | 0.7369 | 0.7963 |
| fcn | mIoU | 0.8783 | 0.0040 | 0.8624 | 0.8935 |
| fcn | accuracy | 0.9899 | 0.0006 | 0.9876 | 0.9920 |
| segnet | Dice | 0.8386 | 0.0353 | 0.8124 | 0.8630 |
| segnet | IoU | 0.7504 | 0.0416 | 0.7180 | 0.7815 |
| segnet | mIoU | 0.8691 | 0.0223 | 0.8520 | 0.8856 |
| segnet | accuracy | 0.9884 | 0.0030 | 0.9858 | 0.9908 |
| unet | Dice | 0.8451 | 0.0135 | 0.8167 | 0.8714 |
| unet | IoU | 0.7612 | 0.0168 | 0.7272 | 0.7935 |
| unet | mIoU | 0.8747 | 0.0089 | 0.8567 | 0.8916 |
| unet | accuracy | 0.9889 | 0.0010 | 0.9864 | 0.9912 |

### Piloto retrospectivo de validación

| Red | mean | std | CV (%) |
|---|---|---|---|
| fcn | 0.8330 | 0.0162 | 1.9456 |
| segnet | 0.8193 | 0.0413 | 5.0376 |
| unet | 0.8272 | 0.0106 | 1.2818 |

Estas cinco semillas son las corridas definitivas, no un piloto independiente. R es la aproximación normal de planificación bilateral para δ=0,02, α=0,05 y potencia 0,80. Es inestable con cinco pares; no modifica el presupuesto acordado.

| Contraste | DE pareada | δ/DE | R estimada |
|---|---|---|---|
| fcn − segnet | 0.0515 | 0.3887 | 52 |
| fcn − unet | 0.0258 | 0.7764 | 14 |
| segnet − unet | 0.0332 | 0.6022 | 22 |

![Historiales por red y semilla](figuras/curvas_v2.png)

Los historiales guardaron pérdida de entrenamiento y Dice por grupo de validación. Son escalas diferentes: su resta no mide sobreajuste. No se registraron Dice de train y pérdida de validación por época.

### Equivalencia condicional a la partición histórica

| Contraste | Δ Dice | IC90 inf. | IC90 sup. | p TOST | p t pareado | p TOST Holm | p t Holm |
|---|---|---|---|---|---|---|---|
| fcn − segnet | 0.0144 | 0.0060 | 0.0229 | 0.1384 | 0.0055 | 0.1384 | 0.0165 |
| fcn − unet | 0.0079 | 0.0017 | 0.0142 | 0.0009 | 0.0382 | 0.0027 | 0.0763 |
| segnet − unet | -0.0065 | -0.0140 | 0.0010 | 0.0017 | 0.1526 | 0.0033 | 0.1526 |

TOST bilateral de equivalencia práctica: dos pruebas unilaterales sobre la media de diferencias por grupo dentro de ±0,02; se ajustan tres contrastes con Holm. Solo p TOST Holm < 0,05 respalda equivalencia. Un resultado inconcluso no prueba diferencia. La prueba t pareada responde a otra pregunta: si la diferencia media es cero. Los grupos se suponen independientes.

### Errores según área anotada

| model | área | mean | size |
|---|---|---|---|
| fcn | ≤2 % | 0.8101 | 47 |
| fcn | 2–5 % | 0.8504 | 60 |
| fcn | >5 % | 0.9038 | 25 |
| segnet | ≤2 % | 0.7989 | 47 |
| segnet | 2–5 % | 0.8334 | 60 |
| segnet | >5 % | 0.9038 | 25 |
| unet | ≤2 % | 0.8107 | 47 |
| unet | 2–5 % | 0.8319 | 60 |
| unet | >5 % | 0.8990 | 25 |

size cuenta grupos representados en cada estrato. Una imagen multiclase conserva una sola máscara binaria: el Dice por categoría presente no es IoU multiclase.

### Margen para una nueva línea base

El objetivo de referencia en validación es 0.8330 + 0,02 = 0.8530 Dice. Interpretar también la diferencia pareada y su incertidumbre. En test histórico, la ventaja relativa de FCN sobre U-Net es 0.94 %. El test ya fue consultado en la versión 1; las nuevas comparaciones sobre él son una ampliación retrospectiva, no validación externa.
