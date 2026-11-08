## Diagnósticos adicionales sobre el corpus

| Categoría | Píxeles | % del total |
|---|---|---|
| poison | 976074 | 0.1267 |
| dangerous | 1430437 | 0.1856 |
| organic-peroxide | 1538311 | 0.1996 |
| corrosive | 1700992 | 0.2207 |
| flammable | 1971806 | 0.2559 |
| spontaneously-combustible | 2036709 | 0.2643 |
| infectious-substance | 2626241 | 0.3408 |
| flammable-solid | 2652673 | 0.3442 |
| non-flammable-gas | 3318110 | 0.4306 |
| inhalation-hazard | 3355659 | 0.4355 |
| explosive | 3811887 | 0.4947 |
| oxygen | 4110704 | 0.5334 |
| radioactive | 4231863 | 0.5492 |
| background | 736827254 | 95.6187 |

Denominador: píxeles de todos los pares con máscara válida, antes de la partición. La categoría poison tiene la menor representación; su conteo no equivale al número de grupos independientes.

### Ausencia de máscaras

En 1685 imágenes con características se observan 102 máscaras ausentes. Asociación exploratoria con la carpeta de origen: χ²=1667.59, gl=7, p=0. Las celdas pequeñas y la dependencia entre imágenes relacionadas limitan la prueba. Una asociación observada cuestiona MCAR; no identifica MAR ni descarta MNAR.

### ¿Se distinguen train y test?

Clasificador logístico de partición con tamaño, brillo, contraste y fracción de etiqueta; predicción fuera de muestra en cinco pliegues agrupados. AUC=0.506 (azar: 0,5). Se usa como diagnóstico retrospectivo, sin cambiar la partición ni entrenar el segmentador con test.

| Variable | D de KS | p nominal por imagen |
|---|---|---|
| width | 0.0226 | 1.0000 |
| height | 0.0257 | 0.9996 |
| brightness_thumb | 0.0737 | 0.2685 |
| contrast_thumb | 0.1261 | 0.0060 |
| foreground_fraction | 0.1240 | 0.0072 |

Los valores p de KS son descriptivos: no corrigen la dependencia intragrupo. La fracción anotada solo se usa para diagnosticar diferencias de composición.

**Decisión de preprocesamiento:** conservar encuadre y proporción con letterbox de 256 × 256; mantener la máscara discreta y excluir relleno. **Impacto en la arquitectura:** combinar detalle local con contexto sin introducir nuevas transformaciones en la comparación. **Riesgos:** los objetos pequeños pierden detalle y la procedencia podría actuar como atajo. La estabilidad del umbral y del grupo de similitud no está garantizada en otro puerto.
