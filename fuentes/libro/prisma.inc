### Flujo de selección y decisiones de los revisores

La {download}`matriz de cribado y extracción <../docs/revision_literatura.xlsx>` conserva un identificador por registro. El diagrama tipo PRISMA sigue los totales por base de la hoja *busquedas* y los recuentos de *LEEME*. Cada rama lateral indica cuántos registros dejaron de avanzar en ese paso.

![Flujo PRISMA. De 653 registros identificados se eliminaron 120 duplicados. De 533 cribados se excluyeron 326. Se buscaron 207 textos, se recuperaron 159 y se incluyeron 96.](figuras/flujo_prisma.png)

En el cribado se excluyeron 19 registros sin aprendizaje profundo, 134 centrados solo en cajas o clasificación, 105 de otro dominio, 63 que no eran estudios primarios y 5 sobre OCR de documentos. De los 159 textos evaluados se excluyeron 28 por abordar solo cajas o clasificación, 15 por dominio ajeno, 2 por tipo de estudio, 13 por carecer de línea base de aprendizaje profundo y 5 por métricas no verificables.

**Quién seleccionó los registros.** La columna *revisor* de *cribado* atribuye la primera decisión sobre los 533 títulos y resúmenes a Claude, con cinco subagentes que aplicaron los criterios de cribado. Daniel Carbonó y Luis Perez figuran conjuntamente como segundo revisor de una muestra ciega de 50. La matriz no identifica por separado quién evaluó la elegibilidad de cada texto completo.

**Cómo se resolvieron las diferencias.** En la muestra hubo 47 coincidencias y tres desacuerdos entre «incluir», «dudoso» y «excluir». Los tres pasaron a búsqueda de texto completo. R282 se excluyó tras la lectura por faltar una línea base de aprendizaje profundo (E6). R295 se excluyó por dominio ajeno (E3). El texto de R041 no se recuperó, por lo que no llegó a evaluarse.

**Acuerdo entre revisores.** Al cruzar los 50 identificadores de *muestra_ciega* con *decision_tr* de *cribado*, el kappa de Cohen es 0,901 con tres categorías (47/50 coincidencias) y 0,960 al agrupar «incluir» y «dudoso» como «pasa» (49/50). La hoja *LEEME* consigna 0,760 y 0,797 respectivamente. Para que el cálculo sea reproducible, aquí se usan las decisiones individuales guardadas. La discrepancia del resumen queda documentada.
