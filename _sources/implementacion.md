# Diseño experimental e implementación

## Cinco maneras de reconstruir una máscara

Cada red recibe la misma fotografía RGB de 256 × 256 y predice un logit por píxel. El codificador sigue un diseño de tres niveles de 16, 32 y 64 canales, con dos reducciones espaciales por máximos. Cada arquitectura aprende sus propios pesos desde cero.

![Mecanismos de reconstrucción de las cinco redes](figuras/arquitecturas_cinco.png)

```{include} mapas_redes.inc
```

Una convolución calcula $h_j=\mathrm{ReLU}(b_j+\sum_i W_{ji}*x_i)$. Para un núcleo de lado $k$, con $C_i$ canales de entrada y $C_o$ de salida, aporta $(k^2 C_i+1)C_o$ parámetros. En un mapa de $H\times W$ su costo es del orden de $HWk^2C_iC_o$ operaciones por imagen. Se suma por capa; la retropropagación conserva el orden y añade gradientes y activaciones intermedias. La tabla de tiempos mide épocas con entrenamiento y validación incluidos.

```{include} fundamentos.inc
```

## Metodología experimental

**De la ecuación al entrenamiento.** En `colab_runner.loss_fn`, `valid` selecciona los píxeles observados antes de sumar. El último renglón asigna el mismo peso a BCE y a la pérdida Dice.

```{literalinclude} ../scripts/colab_runner.py
:language: python
:pyobject: loss_fn
```

**De validación al checkpoint.** En `run`, `score` es el Dice medio por grupo en validación. Una mejora estricta actualiza `best.pt`; en caso de empate se conserva el primer máximo.

```{literalinclude} ../scripts/colab_runner.py
:language: python
:lines: 167-172
:dedent: 8
```

| Elemento | Valor aplicado en las 25 corridas |
|---|---|
| Entrada y lote | 256 × 256, lote 1 |
| Optimizador | Adam, tasa fija 0,001 |
| Pérdida | ½ BCE con logits + ½ pérdida Dice suave, solo píxeles válidos |
| Presupuesto | Máximo 30 épocas, paciencia 5 sin mejora en Dice de validación por grupo |
| Semillas comunes | 11, 23, 37, 53 y 71 |
| Selección | Mejor checkpoint de cada corrida por validación; una evaluación final en test con umbral 0,5 |

La pérdida Dice suave es $1-(2\sum py+\epsilon)/(\sum p+\sum y+\epsilon)$, donde $p$ es la salida sigmoide, $y$ la máscara binaria y $\epsilon=10^{-6}$. El orden de imágenes depende de la semilla y la época. Se fijaron los generadores de Python, NumPy y PyTorch, también en CUDA. La configuración fue fija en las 25 corridas, sin búsqueda de hiperparámetros.

## Hardware y reproducibilidad

FCN, SegNet y U-Net se entrenaron en CPU y T4 con determinismo habilitado en modo de advertencia. Sus tres corridas de semilla 11 y FCN con semilla 23 se ejecutaron en CPU. SegNet con semilla 23 pasó de CPU a T4 tras la primera época. Las demás corridas de estas arquitecturas usaron T4. DeepLab compacta y PSPNet compacta se entrenaron en A100-SXM4-80GB con caché de datos, TF32, selección de algoritmos cuDNN por rendimiento y determinismo desactivado. Estas diferencias limitan la atribución de los resultados exclusivamente a la arquitectura. En Resultados se mide la latencia de las cinco redes en la misma CPU.

Las implementaciones de {download}`FCN, SegNet y U-Net <../scripts/modelos_base.py>` y de {download}`DeepLab y PSPNet compactas <../scripts/bases_v2.py>` definen las cinco arquitecturas. El {download}`cuaderno A100 <../colab/Experimento_IMO_A100_optimizado.ipynb>` y su {download}`archivo de resultados <../colab/IMO_resultados_A100.zip>` conservan configuraciones, pesos e historias. Los {download}`paneles de predicciones <../scripts/paneles_cierre.py>` se reconstruyen con los cinco checkpoints de semilla 11, las imágenes de test, la normalización de entrenamiento y las máscaras verificadas por SHA-256. El {download}`generador de diagramas y curvas <../scripts/figuras_cinco.py>` reúne las cinco arquitecturas.
