# Segmentación binaria de rótulos de mercancías peligrosas: Entregable 1

Libro del Entregable 1 de Deep Learning (Luis Perez y Daniel Carbonó): EDA del corpus HAZMAT-13, revisión
sistemática de la literatura y benchmark de cinco redes de segmentación binaria (FCN, SegNet, U-Net, DeepLab
compacta y PSPNet compacta) con 5 semillas cada una.

**Libro publicado:** https://luisperez03.github.io/Deep_Learning_Segementacion_IMO/

## Qué hay en el repositorio

| Ruta | Contenido |
|---|---|
| `index.html` y demás `.html` | El libro compilado (Jupyter Book). Se abre sin conexión |
| `_sources/` | Texto fuente de cada capítulo |
| `_downloads/.../IMO_cinco_redes.zip` | Archivo experimental: código de las redes, manifiestos, checkpoints, historiales y predicciones de test de las 25 corridas |
| `_downloads/.../revision_literatura.xlsx` | Revisión sistemática completa: búsquedas, cribado de 533 registros, muestra ciega y kappa, extracción de 159, síntesis y calidad de 96, frecuencia de modelos |
| `_downloads/.../Estado_del_arte_IMO_matriz_4_fuentes.xlsx` | Matriz de los antecedentes del dominio (DeepHAZMAT y otros) |

## Archivo experimental `IMO_cinco_redes.zip`

| Carpeta | Contenido |
|---|---|
| `modelos/` | Las cinco redes en PyTorch (`modelos_base.py`, `bases_v2.py`) |
| `protocolo/` | Manifiestos `train.csv`, `val.csv` y `test.csv` (con SHA-256 de cada imagen y máscara, y el grupo de similitud) y la normalización calculada en train |
| `corridas/<red>_seed<s>_lr0.001/` | `config.json` (hiperparámetros y entorno), `history.json` (pérdida y Dice de validación por época), `result.json`, predicciones de validación y el checkpoint elegido |
| `test/` | Métricas de test por imagen y por corrida de las cinco redes |
| `comparacion/` | Tablas conjuntas: benchmark, equivalencia y dimensionamiento, categorías, robustez e inferencia en CPU |
| `sha256_best.json` | Hash de los 25 checkpoints |

## Protocolo

- **Datos:** HAZMAT-13, 1.685 fotografías y 13 categorías, binarizadas en etiqueta y fondo. Hay 1.583 pares con
  máscara; se usan 1.015 en train, 218 en validación y 218 en test, y 132 quedan en cuarentena.
- **Partición:** por grupos de similitud (dHash y aHash), con cada grupo completo en una sola partición. El test
  tiene 122 grupos.
- **Entrada:** 256 × 256 conservando la proporción, con el relleno excluido de la pérdida y de la métrica.
- **Entrenamiento:** Adam con tasa fija de 0,001, lote 1, hasta 30 épocas, parada temprana con paciencia 5
  sobre el Dice de validación por grupos. La pérdida es (BCE + Dice) / 2. Sin aumentos, sin dropout, sin
  preentrenamiento.
- **Semillas:** 11, 23, 37, 53 y 71.
- **Métrica primaria:** Dice de la etiqueta por grupo. δ = 0,02 y α = 0,05.

## Entornos

| Redes | Entrenamiento | Evaluación de test | Versiones |
|---|---|---|---|
| FCN, SegNet, U-Net | CPU | GPU T4 | torch 2.15.0+cpu (entrenamiento), torch 2.14.1+cu130 y numpy 2.4.6 (test) |
| DeepLab, PSPNet compactas | GPU A100 | GPU A100 | torch 2.14.1+cu130, numpy 2.4.6, CUDA 13.0, NVIDIA A100-SXM4-80GB, TF32 y cuDNN benchmark |

Las versiones exactas de cada corrida están en su `config.json`; las del test de FCN, SegNet y U-Net, en
`test/FCN_SegNet_UNet/procedencia.json`.

## Cómo reproducir

1. Descargar HAZMAT-13 y comprobar que los SHA-256 de imágenes y máscaras coinciden con los de
   `protocolo/*.csv`.
2. Entrenar cada red y semilla con la configuración de `corridas/*/config.json`. **[Por completar por Luis:
   script y comando de entrenamiento.]**
3. Evaluar el checkpoint de cada corrida sobre `protocolo/test.csv`. **[Por completar: script y comando de
   evaluación.]**
4. Recalcular las tablas de `comparacion/` a partir de `test/*/por_imagen.csv`.

Para verificar los resultados sin reentrenar, basta con el paso 4: las predicciones de test de las 25 corridas
están en el archivo.

## Limitaciones conocidas

Son cinco semillas por red, y el dimensionamiento pide entre 4 y 52 según el par. No hubo búsqueda de
hiperparámetros ni validación cruzada repetida. Las redes se ejecutaron en dos entornos de hardware. No hay
segunda anotación del corpus. Ver el capítulo de resultados.
