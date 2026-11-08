## Variación entre semillas y equivalencia práctica

La planificación usa el mejor Dice de validación de cada corrida. Con cinco semillas comunes, la media, la desviación estándar (DE) y su cociente (CV) describen la estabilidad observada bajo la misma partición.

| Red | Media | DE | CV (%) |
|---|---:|---:|---:|
| FCN | 0,8330 | 0,0162 | 1.95 |
| SegNet | 0,8193 | 0,0413 | 5.04 |
| U-Net | 0,8272 | 0,0106 | 1.28 |
| DeepLab | 0,8479 | 0,0178 | 2.10 |
| PSPNet | 0,8442 | 0,0135 | 1.60 |

La equivalencia usa dos pruebas unilaterales (TOST) sobre la diferencia media de Dice entre grupos, con margen ±0,02 y semillas fijas. Holm corrige los diez contrastes de esta familia exploratoria. R aproxima las semillas necesarias para detectar 0,02 con potencia 0,80 y α bilateral 0,05 usando la DE pareada de validación. Cinco observaciones dan una estimación imprecisa de esa DE.

| Par | Δ Dice test | IC 90 % | p TOST Holm | R aprox. |
|---|---:|---|---:|---:|
| FCN − SegNet | 0,0144 | 0,0060 a 0,0229 | 0,7915 | 52 |
| FCN − U-Net | 0,0079 | 0,0017 a 0,0142 | 0,0082 | 14 |
| FCN − DeepLab | -0,0081 | -0,0167 a 0,0004 | 0,0818 | 8 |
| FCN − PSPNet | -0,0136 | -0,0230 a -0,0042 | 0,7915 | 4 |
| SegNet − U-Net | -0,0065 | -0,0140 a 0,0010 | 0,0133 | 22 |
| SegNet − DeepLab | -0,0226 | -0,0304 a -0,0147 | 1,0000 | 39 |
| SegNet − PSPNet | -0,0280 | -0,0367 a -0,0194 | 1,0000 | 44 |
| U-Net − DeepLab | -0,0161 | -0,0249 a -0,0072 | 0,9278 | 9 |
| U-Net − PSPNet | -0,0216 | -0,0320 a -0,0111 | 1,0000 | 9 |
| DeepLab − PSPNet | -0,0055 | -0,0109 a -0,0001 | 0,0001 | 2 |

Un p TOST ajustado inferior a 0,05 respalda equivalencia dentro del margen. Los contrastes de superioridad frente a FCN del benchmark responden a si la diferencia media es distinta de cero. Los diez contrastes se detallan en `comparacion/equivalencia_y_dimensionamiento.csv` del ZIP.
