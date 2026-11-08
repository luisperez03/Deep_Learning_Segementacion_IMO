| Caso | Red | VP | FP | FN | Dice CPU | Dice del ZIP |
|---|---|---:|---:|---:|---:|---:|
| image_1118 | FCN | 2485 | 45 | 49 | 0.98144 | 0.98144 |
| image_1118 | SegNet | 2517 | 97 | 17 | 0.97786 | 0.97786 |
| image_1118 | U-Net | 2534 | 121 | 0 | 0.97668 | 0.97668 |
| image_1118 | DeepLab compacta | 2428 | 12 | 106 | 0.97628 | 0.97628 |
| image_1118 | PSPNet compacta | 2514 | 30 | 20 | 0.99015 | 0.99015 |
| image_1241 | FCN | 481 | 68 | 20 | 0.91619 | 0.91619 |
| image_1241 | SegNet | 490 | 316 | 11 | 0.74981 | 0.74981 |
| image_1241 | U-Net | 474 | 91 | 27 | 0.88931 | 0.88931 |
| image_1241 | DeepLab compacta | 407 | 52 | 94 | 0.84792 | 0.84792 |
| image_1241 | PSPNet compacta | 483 | 66 | 18 | 0.92000 | 0.92000 |
| image_1656 | FCN | 17 | 256 | 713 | 0.03390 | 0.03390 |
| image_1656 | SegNet | 54 | 271 | 676 | 0.10237 | 0.10237 |
| image_1656 | U-Net | 0 | 1116 | 730 | 0.00000 | 0.00000 |
| image_1656 | DeepLab compacta | 1 | 890 | 729 | 0.00123 | 0.00123 |
| image_1656 | PSPNet compacta | 0 | 0 | 730 | 0.00000 | 0.00000 |

Diferencia absoluta máxima entre CPU y el registro GPU en las cuatro métricas de las quince predicciones: 0. El cotejo es por métricas; el ZIP no contiene máscaras GPU para certificar identidad píxel a píxel.
