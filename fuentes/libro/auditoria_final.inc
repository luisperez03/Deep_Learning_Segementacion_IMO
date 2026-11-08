## Robustez, coste de inferencia y errores

Se sortearon 24 de los 122 grupos de test (semilla 20261108) y una imagen por grupo. Los mismos 25 checkpoints procesaron cada imagen sin perturbación y con tres cambios por separado: brillo ×1,25, ruido gaussiano de DE 12 sobre la escala 0–255 y desenfoque gaussiano de radio 2 píxeles. Son diagnósticos internos. La mayor diferencia absoluta entre el Dice original recalculado en CPU y el registrado en el ZIP para esas imágenes fue 0,0009.

| Red | Dice original | Δ brillo | Δ ruido | Δ desenfoque |
|---|---|---|---|---|
| PSPNet compacta | 0,8877 | -0,0224 | -0,0016 | -0,2330 |
| DeepLab compacta | 0,8881 | -0,0267 | -0,0013 | -0,2185 |
| FCN | 0,8755 | -0,0384 | -0,0007 | -0,3132 |
| U-Net | 0,8676 | -0,0358 | 0,0003 | -0,2578 |
| SegNet | 0,8588 | -0,0366 | 0,0012 | -0,2219 |

### Coste por fotografía

| Red | Mediana (ms) | P90 (ms) | GMAC convoluciones |
|---|---|---|---|
| PSPNet compacta | 24.25 | 26.76 | 0.65 |
| DeepLab compacta | 28.87 | 31.19 | 0.76 |
| FCN | 21.81 | 29.26 | 0.63 |
| U-Net | 57.39 | 64.14 | 1.84 |
| SegNet | 39.36 | 52.28 | 1.06 |

Medición local en CPU con dos hilos y lote 1, después de un calentamiento por checkpoint. Se cronometró únicamente la pasada hacia delante en 120 observaciones por arquitectura. Los GMAC cuentan multiplicaciones y acumulaciones de las convoluciones (una MAC equivale aproximadamente a dos FLOP); quedan fuera pooling, interpolaciones, transferencias y preprocesamiento.

### Falsos positivos y omisiones

| Red | FP por imagen | FN por imagen |
|---|---|---|
| PSPNet compacta | 258.0 | 115.1 |
| DeepLab compacta | 236.9 | 125.5 |
| FCN | 233.5 | 168.8 |
| U-Net | 296.0 | 142.9 |
| SegNet | 283.2 | 157.0 |

Conteos medios de píxeles válidos de 256 × 256 sobre la submuestra sin perturbación. Un FP extiende la predicción a fondo anotado y un FN omite parte del rótulo.

### Categorías presentes y casos difíciles

Se realizaron 26 contrastes exploratorios entre FCN y las nuevas bases, uno por modelo y categoría presente. Cada imagen conserva su máscara binaria completa. Se promediaron imágenes dentro del grupo y luego semillas. Con ajuste de Holm sobre toda la familia, 0 contrastes tuvieron p ajustado inferior a 0,05. La tabla `comparacion/categorias.csv` del ZIP incluye los tamaños de muestra y las diferencias. 

Cinco imágenes con menor Dice medio en las 25 corridas. Los ejemplos se ordenaron sobre todas las imágenes de test.

| Imagen | Grupo | Dice medio |
|---|---|---|
| image_0538 | g_image_0538 | 0,2105 |
| image_0542 | g_image_0145 | 0,3816 |
| image_1656 | g_image_1656 | 0,4033 |
| image_1541 | g_image_0538 | 0,4097 |
| image_0380 | g_image_0380 | 0,4794 |

### Fuentes de variación

En la tabla equilibrada de 5 redes × 5 semillas × 122 grupos, la suma de cuadrados del Dice se distribuye en 0.4 % para diferencias medias entre redes, 0.4 % para semillas y 76.2 % para grupos. El 23.0 % restante reúne interacciones. Es una descomposición descriptiva de esta partición. Los grupos concentran la mayor parte de la variabilidad.

El contraste global de Friedman, bloqueado por grupo y con semillas fijas, dio Q=67.51 y p=7.62e-14. 

Comparaciones exploratorias de Nemenyi para las cinco redes. Los contrastes primarios frente a FCN se definieron por separado frente a FCN.

| Red | Comparador | p Nemenyi |
|---|---|---|
| FCN | U-Net | 0,7662 |
| FCN | SegNet | 0,0120 |
| FCN | DeepLab compacta | 0,0034 |
| FCN | PSPNet compacta | 0,0008 |
| U-Net | SegNet | 0,0001 |
| U-Net | DeepLab compacta | 0,1183 |
| U-Net | PSPNet compacta | 0,0466 |
| SegNet | DeepLab compacta | 0,0000 |
| SegNet | PSPNet compacta | 0,0000 |
| DeepLab compacta | PSPNet compacta | 0,9962 |

Los resultados por imagen y perturbación se conservan en `comparacion/robustez_24_grupos.csv` del ZIP.
