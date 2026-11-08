# Segmentación de etiquetas de mercancías peligrosas

**Deep Learning · Entregable 1 · Noviembre de 2026**

**Autores:** Luis Perez y Daniel Carbono

{ref}`Ver cuántas imágenes hay de cada clase <conteo-por-categoria>`.

¿Con qué precisión se puede delimitar un rótulo de peligro cuando las fotografías relacionadas se mantienen en una misma partición? Comparamos cinco líneas base sobre HAZMAT-13. DeepLab compacta lideró en validación y PSPNet compacta en test. FCN es el comparador de referencia. Cada arquitectura se entrena con las mismas cinco semillas.

![Diseño agrupado de las 25 corridas](figuras/flujo_cinco.png)

Las tres ramas del diagrama tienen funciones distintas. Entrenamiento ajusta los pesos; validación elige qué pesos conservar; test mide el desempeño del checkpoint elegido. Cada grupo de fotos relacionadas sigue una sola rama.

```{tableofcontents}
```

## Resumen ejecutivo

```{include} ../docs/RESUMEN_EJECUTIVO.md
:start-line: 1
```

El {download}`resumen ejecutivo <../docs/RESUMEN_EJECUTIVO.md>` recoge los resultados principales.