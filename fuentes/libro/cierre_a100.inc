## Comparación final de cinco líneas base

DeepLab compacta obtuvo el mayor Dice medio de validación (0,8479). En test, PSPNet compacta encabeza las cinco redes (0,8667), seguida de DeepLab compacta (0,8612) y FCN (0,8531). Las dos mejoras medias frente a FCN quedan por debajo del margen práctico de 0,02. Los intervalos pareados permiten valorar la incertidumbre alrededor de esas mejoras.

| Red | Dice val. | Dice test | DE semillas | IC 95 % Dice | IoU | mIoU | Parámetros |
|---|---|---|---|---|---|---|---|
| PSPNet compacta | 0,8442 | 0,8667 | 0,0078 | 0,8444 a 0,8870 | 0,7851 | 0,8875 | 80401 |
| DeepLab compacta | 0,8479 | 0,8612 | 0,0217 | 0,8374 a 0,8837 | 0,7799 | 0,8848 | 104481 |
| FCN | 0,8330 | 0,8531 | 0,0063 | 0,8284 a 0,8761 | 0,7673 | 0,8783 | 72145 |
| U-Net | 0,8272 | 0,8451 | 0,0135 | 0,8167 a 0,8714 | 0,7612 | 0,8747 | 118273 |
| SegNet | 0,8193 | 0,8386 | 0,0353 | 0,8124 a 0,8630 | 0,7504 | 0,8691 | 85761 |

Cada imagen se pondera dentro de su grupo y cada grupo recibe el mismo peso. El Dice de validación corresponde al mejor checkpoint de cada semilla. La DE describe la variación entre cinco entrenamientos. Los IC percentiles de test se calcularon con 10.000 remuestreos de los 122 grupos y semillas fijas.

### Contrastes previstos frente a FCN

| Modelo menos FCN | Δ Dice | IC 95 % pareado | p t pareado | p Holm |
|---|---|---|---|---|
| DeepLab compacta | 0,0081 | -0,0016 a 0,0187 | 0,1183 | 0,1183 |
| PSPNet compacta | 0,0136 | 0,0032 a 0,0250 | 0,0181 | 0,0361 |

Las pruebas t bilaterales contrastan la diferencia media entre grupos, después de promediar las cinco semillas. Holm ajusta los dos valores p. Los IC son individuales. La inferencia supone independencia entre grupos y se restringe a esta partición y a estos entrenamientos.

### Tamaño del rótulo y errores

| Modelo | ≤2 % | 2–5 % | >5 % |
|---|---|---|---|
| PSPNet compacta | 0,8397 | 0,8591 | 0,9192 |
| DeepLab compacta | 0,8326 | 0,8563 | 0,9049 |
| FCN | 0,8101 | 0,8504 | 0,9038 |
| U-Net | 0,8107 | 0,8319 | 0,8990 |
| SegNet | 0,7989 | 0,8334 | 0,9038 |

Los estratos se definen por la fracción anotada de cada imagen. Un grupo puede estar representado en más de un estrato. Los objetos pequeños son los más difíciles para las cinco redes. En condiciones portuarias, este patrón importa porque los rótulos distantes ocupan menos píxeles.

### Curvas de las 25 corridas

![Pérdida y Dice por época de las cinco redes](figuras/curvas_cinco.png)

Se eligió el primer máximo de Dice por grupo en validación. Las curvas terminan al alcanzar 30 épocas o al acumular cinco épocas sin mejora. La pérdida de entrenamiento y el Dice de validación tienen escalas distintas.

### Coste observado y alcance

Tiempo medio registrado por corrida en A100, incluyendo entrenamiento y validación: DeepLab compacta, 2.9 min, PSPNet compacta, 3.0 min. FCN, SegNet y U-Net se entrenaron en CPU y T4. Las diferencias de duración mezclan arquitectura, hardware, caché y perfil numérico. El conteo de parámetros sí se puede comparar directamente.

FCN conserva su papel de referencia del benchmark. PSPNet compacta ofrece el mayor Dice observado en test y su mejora media queda por debajo de δ. La validación sobre otros puertos y otras particiones sigue abierta. El diseño aporta cinco semillas por arquitectura, una partición agrupada y configuraciones fijas. No se realizó búsqueda de hiperparámetros, validación cruzada repetida ni una segunda anotación independiente.

Las métricas de las 25 corridas y el resumen con intervalos se encuentran en `comparacion/metricas_25_corridas.csv` y `comparacion/benchmark.csv` del {ref}`archivo de las cinco redes <archivo-cinco-redes>`.
