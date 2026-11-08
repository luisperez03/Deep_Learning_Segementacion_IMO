# EDA y preparación

Este capítulo integra la exploración inicial con los manifiestos y las máscaras usados en las 25 corridas.

(conteo-por-categoria)=
## Cuántas imágenes contienen cada categoría

La tabla cuenta fotografías entre los **1.583 pares con máscara utilizable**. Una foto con dos rótulos de la misma categoría cuenta una vez en esa fila. Si contiene dos categorías diferentes, cuenta en ambas filas; por eso la suma de las filas puede superar 1.583. Las 102 imágenes sin máscara quedan fuera de este recuento.

```{include} conteo_clases.inc
```

La columna **Total con máscara** reúne entrenamiento (1.015 fotos), validación (218), test (218) y cuarentena (132). Para reconstruir los conteos se cruzó el campo `classes` de `resultados/inicio_voc/inventario.csv` con el manifiesto por ID de imagen y se cotejó cada total con `distribucion_clases.csv`. El fondo es la categoría negativa por píxel y queda fuera de las 13 categorías de rótulos.


```{admonition} Decisión, impacto y riesgos
:class: note

- *Decisión de preprocesamiento:* binarizar las 13 categorías en una sola clase "etiqueta"; la partición
  conserva la presencia de todas las categorías en train, validación y test.
- *Impacto en la arquitectura:* salida de un canal con sigmoide; no hace falta una cabeza multiclase.
- *Riesgos:* las categorías poco frecuentes (Tóxico, 81 imágenes) pesan poco en el entrenamiento. Un error
  sistemático en ellas quedaría diluido en el Dice global, y por eso se reporta el Dice por categoría.
```

## Cobertura, geometría y objetivo

### Las 13 categorías que aparecen en las máscaras originales

¿Qué cuenta como «etiqueta» para la red? En la galería, cada fotografía es un recorte real de entrenamiento. El título traduce el nombre de la categoría; debajo aparece el identificador exacto de la paleta VOC y de la imagen. Para el experimento binario, los píxeles anotados de cualquiera de estas 13 categorías se convierten en **1 (etiqueta)** y el fondo en **0**. Los nombres proceden del corpus y su equivalencia con divisiones IMO requeriría una revisión específica.

```{include} galeria_clases.inc
```

**Cómo se eligieron los ejemplos.** Para cada categoría se tomó la fotografía elegible de entrenamiento con mayor fracción anotada entre las que tienen solo esa categoría. El recorte abarca sus píxeles y añade un margen de contexto. Así se aprecia el pictograma; la galería sobrerrepresenta deliberadamente los rótulos grandes. Los gráficos siguientes cuantifican el tamaño y el desbalance en las imágenes completas.

### Cuánto espacio ocupa una etiqueta

![Fotografía de entrenamiento, regiones de dos categorías y unión binaria de las máscaras](figuras/objetivo_binario.png)

En `image_1165`, la máscara original distingue sólido inflamable y radiactivo. La columna central usa dos colores de visualización para hacer legible esa diferencia. A la derecha se muestra la máscara binaria guardada que recibió el modelo. Conserva las dos regiones, incluso cuando ya no se puede deducir la clase a partir de sus valores.

La función que construyó la máscara comprueba la paleta antes de asignar 1 a los píxeles anotados. El valor RGB empaquetado cero corresponde al fondo negro.

```{literalinclude} ../scripts/preparar_base.py
:language: python
:pyobject: binary_mask
```

![Proporciones de fondo y etiqueta en los pares originales](figuras/08_balance_pixeles.png)

El fondo domina el recuento. Un clasificador que declarase fondo en todos los píxeles alcanzaría una exactitud alta y perdería el rótulo entero. Se eligió Dice de etiqueta como métrica primaria y se combinó Dice suave con BCE en la pérdida.

**De los conteos al porcentaje.** `eda_entregable.py` lee los píxeles por categoría de los 1.583 pares válidos en `resultados/inicio_voc/resumen.json`. La variable `fg` es la fracción de píxeles de etiqueta; multiplicarla por 100 produce el 4,38 % reportado.

```{literalinclude} ../scripts/eda_entregable.py
:language: python
:lines: 31-33
:dedent: 4
```

![Tamaño relativo de etiquetas en las imágenes](figuras/07_objetos_pequenos.png)

Las imágenes con área positiva pequeña son especialmente sensibles a la reducción espacial. La medida se refiere a la suma de área etiquetada por imagen. Más abajo se cuantifican también las regiones individuales que llegan al modelo.

![Relaciones de aspecto del corpus](figuras/05_aspecto.png)

Los encuadres tienen distintas proporciones. Se preservó la relación de aspecto con redimensionado y relleno centrado. Imagen y máscara comparten la geometría; la imagen usa interpolación bilineal y la máscara, vecino más cercano. Los píxeles de relleno se excluyen del entrenamiento y la evaluación.


```{admonition} Decisión, impacto y riesgos
:class: note

- *Decisión de preprocesamiento:* redimensionado a 256 × 256 conservando la proporción, con relleno excluido de
  la pérdida y de la métrica; la máscara se interpola por vecino más cercano.
- *Impacto en la arquitectura:* con 4,38 % de píxeles de etiqueta, la pérdida combina BCE y Dice, y la métrica
  primaria es el Dice de la etiqueta, no la exactitud.
- *Riesgos:* reducir la imagen borra rótulos pequeños (el área mínima por imagen baja a 16 píxeles). Las redes
  que reducen mucho el mapa (FCN, 4 veces) quedan en desventaja en esos casos.
```

## Calidad de las anotaciones

![Fotografía, máscara y revisión de cobertura del caso image_1461](figuras/revision_1461.png)

En la cesta se distingue un rótulo aparentemente omitido mientras dos rótulos inferiores sí tienen máscara. Esta clase de error persiste al binarizar. Se puso el grupo en cuarentena. En `image_0025` la discrepancia parece semántica y en `image_1251` la mano oculta parte de las etiquetas. Los paneles se eligieron para diagnóstico, por lo que su frecuencia de problemas no estima una tasa de error poblacional.

De los 1.685 JPG, 102 carecen de máscara utilizable. Los 102 figuran sin asignación en las particiones heredadas y con origen XML desconocido. Entre los 1.583 pares con máscara solo uno tiene origen desconocido. La ausencia se concentra en un subconjunto identificable; estos metadatos no bastan para explicar la causa. Se excluyeron de la supervisión, sin convertirlos en ejemplos de fondo. En 69 pares no coinciden los conjuntos de clases XML y máscara. También falta el XML de `image_1018`. La cuarentena incluye alertas de cobertura inferior al 5 % en cajas y se propaga dentro del grupo. No existe una segunda anotación independiente para estimar acuerdo o un techo de Dice humano.


```{admonition} Decisión, impacto y riesgos
:class: note

- *Decisión de preprocesamiento:* las 102 imágenes sin máscara se excluyen, sin convertirlas en fondo; los
  grupos con omisiones evidentes van a cuarentena.
- *Impacto en la arquitectura:* ninguno directo. El ruido de etiqueta pone un techo al Dice alcanzable por
  cualquier red.
- *Riesgos:* no hay tasa de ruido estimada ni segunda anotación; un rótulo omitido en test penaliza a la red
  que sí lo detecta. La ausencia de máscaras se concentra por carpeta (χ², p < 10⁻³⁰⁰), así que no es MCAR, y
  excluirlas puede sesgar la muestra hacia el estilo de ciertas carpetas.
```

## Similitud, procedencia y fugas

![Pares de fotografías con firmas perceptuales próximas](figuras/11_similitud.png)

De 1.418.770 pares examinados, 748 cumplieron simultáneamente distancias dHash y aHash de hasta 6 bits sobre 64. De ellos, 146 cruzaban entrenamiento y validación heredados, aunque no se encontraron JPG ni matrices RGB exactamente iguales. Doce pares revisados mostraron composiciones casi coincidentes. La decisión fue mantener juntas las componentes conexas de todos los enlaces candidatos. En la nueva asignación cruzan particiones **cero enlaces conocidos**. Rotaciones y recortes podrían escapar a estos hashes.

En la validación heredada, 33 imágenes provenían de la carpeta XML `00_javad` y 151 de `00_misaq`. Una carpeta puede compartir fondos con otra. Esta concentración y la falta de identificadores certificados de cámara o escena limitan la interpretación del origen. Un grupo visual aproxima dependencia sin garantizarla.


```{admonition} Decisión, impacto y riesgos
:class: note

- *Decisión de preprocesamiento:* agrupar por componentes conexas de dHash y aHash y asignar cada grupo
  completo a una sola partición; el grupo es la unidad de evaluación.
- *Impacto en la arquitectura:* ninguno en el diseño de las redes. Las comparaciones y los intervalos se
  calculan sobre grupos (n = 122), no sobre imágenes.
- *Riesgos (fuga):* rotaciones y recortes pueden escapar a los hashes, y la concentración por carpeta puede
  compartir fondos entre particiones. Una fuga residual inflaría el Dice de test de todas las redes por igual.
```

```{include} eda_calculos.inc
```


```{include} diagnostico_v2.inc
```

## Cierre de los análisis de regiones y particiones

```{admonition} Decisión, impacto y riesgos
:class: note

- *Decisión de preprocesamiento:* normalizar con medias y desviaciones de los píxeles válidos de train solamente.
- *Impacto en la arquitectura:* la mediana de una región por imagen y áreas de 219 a 2.944 píxeles (percentiles
  10 y 90) piden campos receptivos de varias escalas, que es justo lo que añaden DeepLab y PSPNet.
- *Riesgos:* las intensidades de etiqueta y fondo se solapan, así que un atajo por color no basta. Una red que
  dependa del contexto puede fallar con rótulos fuera de su entorno habitual.
```


```{admonition} Decisión, impacto y riesgos
:class: note

- *Decisión de preprocesamiento:* mantener la partición congelada y reportar su diferencia de distribución, en
  vez de rehacerla después de ver los resultados.
- *Impacto en la arquitectura:* ninguno. El test resultó algo más fácil (todas las redes rinden entre 1,3 y 2,3
  puntos más que en validación), pero el desplazamiento es parecido en las cinco y no cambia el orden.
- *Riesgos:* el clasificador adversarial no distingue train de test (AUC 0,506), pero la fracción de etiqueta sí
  difiere (KS p = 0,007). El Dice absoluto de test puede sobrestimar el desempeño fuera de este corpus.
```

