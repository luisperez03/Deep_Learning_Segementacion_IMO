# Resultados, baseline y predicciones

Evaluamos cinco redes con cinco semillas comunes sobre 218 fotografías de test. El cálculo pondera por igual los 122 grupos: primero se promedian las imágenes de cada grupo, después los grupos y finalmente las semillas. Las 5.450 evaluaciones imagen–modelo–semilla pertenecen a un único test interno. Dice e IoU miden el solapamiento del rótulo, la mIoU promedia IoU de rótulo y fondo, y la exactitud cuenta píxeles válidos correctamente clasificados.

```{include} cierre_a100.inc
```

## Lectura de las predicciones

Las filas corresponden a tres casos preseleccionados según el Dice de FCN (alto, central y bajo). Cada columna usa el checkpoint de mejor validación para la semilla 11 de esa arquitectura. La foto, la referencia, el umbral de 0,5 y el área válida son comunes. El ranking global utiliza todas las imágenes y las cinco semillas.

![Fotografía, anotación y cinco máscaras predichas sobre los mismos casos](figuras/comparacion.png)

**Caso alto · image_1118.** Las cinco redes delimitan casi todo el rótulo. PSPNet obtiene Dice 0,990: añade 30 píxeles de fondo y omite 20 del rótulo. DeepLab añade solo 12 píxeles de fondo y omite 106 del objeto. U-Net cubre todos los píxeles anotados y extiende la máscara a 121 píxeles de fondo.

**Caso central · image_1241.** PSPNet y FCN tienen Dice muy cercano (0,920 y 0,916). SegNet recupera 490 píxeles del rótulo, aunque sus 316 falsos positivos reducen el Dice a 0,750. DeepLab produce menos falsos positivos que las demás redes (52) y también omite más píxeles del rótulo (94).

**Caso difícil · image_1656.** De los 730 píxeles anotados, SegNet recupera 54, FCN 17 y DeepLab solo uno. U-Net y PSPNet obtienen Dice cero por razones distintas: U-Net marca 1.116 píxeles de fondo y PSPNet deja vacía la predicción.

![Mapas de error de las cinco arquitecturas](figuras/errores.png)

El verde marca aciertos sobre el rótulo, el naranja muestra dónde la predicción se extiende al fondo y el azul identifica las partes omitidas. Se excluyen las bandas de relleno.

![Contornos anotados y predichos de las cinco redes](figuras/contornos.png)

El borde interior de un píxel se dibuja en azul para la anotación, naranja para la predicción y blanco en las coincidencias. En la última fila el error dominante es de localización.

```{include} conteos_panel.inc
```

```{include} auditoria_final.inc
```

```{include} contrastes_cinco.inc
```

## Decisión para la siguiente etapa

DeepLab compacta fue la mejor en validación (0,8479), el criterio definido para seleccionar un modelo. PSPNet compacta alcanzó el mayor Dice en el test interno (0,8667). Su diferencia media frente a FCN fue +0,0136, con IC 95 % de +0,0032 a +0,0250 y p ajustado de 0,0361. El intervalo abarca el margen práctico de +0,02. Hay evidencia de una ventaja media sobre FCN en estos grupos, y persiste incertidumbre sobre si alcanza la magnitud que se había fijado. Para DeepLab, el IC de la diferencia incluye cero.

El desenfoque produjo la mayor pérdida de Dice entre las perturbaciones ensayadas. En las 24 imágenes de la auditoría, PSPNet perdió 0,2330 y FCN perdió 0,3132. Esto orienta la recolección de fotografías con rótulos distantes y bordes poco definidos. Para el próximo estudio, FCN sirve como comparador, DeepLab como candidato elegido por validación y PSPNet como candidato cuya ventaja se debe comprobar en nuevos grupos.

## Alcance y limitaciones

| Aspecto | Evidencia y límite |
|---|---|
| Procedencia y anotación | Los manifiestos congelan las copias utilizadas mediante SHA-256. La revisión visual previa identifica errores concretos. Quedan sin estimar la tasa poblacional de ruido y el acuerdo con un segundo anotador. Tampoco se certificó la identidad con una versión externa del corpus. |
| Diseño e independencia | Se usaron cinco semillas comunes y una partición agrupada. El piloto retrospectivo permite estimar la dispersión y dimensionar otro estudio. La validación cruzada agrupada repetida, una sexta semilla y un test externo quedan fuera del diseño ejecutado. |
| Entrenamiento | Las configuraciones se fijaron sin búsqueda de hiperparámetros. Los historiales permiten estudiar pérdida de entrenamiento y Dice de validación. Faltan mediciones por época de Dice de entrenamiento y pérdida de validación para cuantificar directamente la brecha. |
| Inferencia e interpretación | El benchmark incluye las cuatro métricas con dispersión e intervalos, contrastes pareados, equivalencia entre arquitecturas, coste, segmentación por tamaño y diagnóstico de robustez. La latencia se midió en CPU y las perturbaciones sobre una submuestra de test. Las métricas por categoría describen presencia en imágenes con máscara binaria. |

