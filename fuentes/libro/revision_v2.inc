## Revisión sistemática

```{include} prisma.inc
```

### Del corpus a la elección de arquitecturas

El resumen de la matriz clasifica 82 artículos como texto en escena y 14 como señales, rótulos o letreros. En el subconjunto de señales, la hoja de frecuencias cuenta seis comparaciones con Mask R-CNN, tres con DeepLab, dos con PSPNet y FCN y una con U-Net. Un estudio puede comparar varias redes. La frecuencia describe uso, no superioridad. En particular, EAST aparece en 40 estudios en total, pero solo en uno del subconjunto de señales. La frecuencia global responde principalmente a la composición del corpus.

| Registro de la síntesis | Resultado extraído | Qué permite comparar |
|---|---|---|
| R455, Zhong et al. (2019), CTSD | IoU 77,17 % en todas las escalas | Motiva estudiar el tamaño del objeto; CTSD no es el test de HAZMAT-13. |
| R522, Yang et al. (2018), percepción de intersecciones | mIoU 67,09 % en 11 clases de navegación | El contexto a varias escalas es pertinente, pero ese promedio no equivale al Dice del rótulo. |
| R330, Qian et al. (2021), TT-100K-HHU | mask mAP 0,644 con aumento de datos | La evaluación exige identificar instancias. No sirve como techo del Dice binario. |
| R070, Tan et al. (2025), placas eléctricas | Reconocimiento de muestras inclinadas: 94,3 % | La cercanía del objeto no hace comparable una tasa de reconocimiento con el solapamiento de máscaras. |

Las cinco redes comparten un diseño de codificador de 16/32/64 canales. FCN, U-Net y SegNet comparan mecanismos de reconstrucción espacial. DeepLab compacta contrasta un mapa grueso con ramas de convolución dilatada y contexto global ([Chen et al., DeepLabv3](https://arxiv.org/abs/1706.05587)). PSPNet compacta agrega promedios regionales a cuatro escalas ([Zhao et al., PSPNet](https://arxiv.org/abs/1612.01105)). Son adaptaciones locales; sus cifras no representan las implementaciones originales. Mask R-CNN queda como antecedente de segmentación por instancias: entrenarla con cajas, asignaciones y pérdidas por instancia cambiaría la comparación solicitada.

La brecha que aborda el experimento es específica: delimitar el rótulo en HAZMAT-13 con grupos separados y comparar redes bajo un presupuesto fijo. Ninguno de los valores ilustrados se puede usar directamente como umbral de superioridad. Las métricas, las particiones y las unidades evaluadas son diferentes. La hoja «calidad» registra por estudio protocolo, fuga, repeticiones y análisis estadístico; «extraccion» conserva motivos de exclusión y los datos que faltan.

