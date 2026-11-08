## Ecuaciones de las cinco arquitecturas

Las cinco redes son CNN para segmentación semántica binaria. Para cada píxel válido, la salida $z$ es un logit y la decisión es $\hat y=\mathbf 1[\sigma(z)\geq 0{,}5]$. Los diagramas de cada red en este capítulo siguen el mismo recorrido que las ecuaciones.

Escribimos $C_k$ para una convolución aprendida de lado $k$ con sesgo, $\rho(t)=\max(0,t)$ y $B(X)=\rho(C_3^{(2)}(\rho(C_3^{(1)}(X))))$. Cada bloque tiene pesos propios. Si $M$ toma el máximo por canal en ventanas de $2\times2$ con paso 2, el codificador de las cinco redes se expresa como

$$
A=B_1(X),\qquad B=B_2(M(A)),\qquad H=B_3(M(B)).
$$

En orden canales × alto × ancho, $A$ mide $16\times256\times256$, $B$ mide $32\times128\times128$ y $H$ mide $64\times64\times64$. Usamos $U_s$ para interpolación bilineal hasta $s\times s$ con `align_corners=False`, $N_s$ para ampliación por vecino más cercano y $[\cdot]$ para concatenación por canales.

### FCN · predicción desde el mapa grueso

$$
z=U_{256}\big(C_1^{64\to1}(H)\big).
$$

La cabeza puntúa el rótulo en una rejilla de 64 × 64 y cada logit final interpola los puntajes vecinos. El diseño supone que esa rejilla conserva suficiente información espacial. Favorece máscaras suaves y puede perder un borde fino antes de la ampliación.

### SegNet · reconstrucción con índices de máximos

Sean $I_1,I_2$ las posiciones elegidas al reducir $A,B$. El operador $S_I$ coloca cada valor en la posición guardada de su ventana y rellena el resto con cero. Con bloques de decodificación $D_2$ y $D_1$,

$$
T=D_2\big(S_{I_2}(C_1^{64\to32}(H))\big),\qquad
F=D_1\big(S_{I_1}(T)\big),\qquad z=C_1^{16\to1}(F).
$$

$D_2$ devuelve 16 canales a 128 × 128 y $D_1$ conserva esos canales a 256 × 256. El diseño supone que la posición de la activación máxima da una pista fiable sobre dónde reconstruir la etiqueta. Las convoluciones completan las posiciones que quedaron vacías.

### U-Net · recuperación de detalle mediante concatenación

$$
T=D_2([N_{128}(H),B]),\qquad F=D_1([N_{256}(T),A]),\qquad z=C_1^{16\to1}(F).
$$

La primera concatenación reúne 64 + 32 canales y la segunda reúne 32 + 16. El diseño supone que los rasgos superficiales alineados con el píxel ayudan a recuperar contornos. También pueden transmitir al decodificador bordes del fondo.

### DeepLab compacta · convolución dilatada y contexto global

Para dilatación $d$, la rama de 3 × 3 calcula en la posición $u$ y el canal $j$

$$
(C_{3,d}H)_j(u)=b_j+\sum_c\sum_{r\in\{-1,0,1\}^2} W_{jcr}H_c(u+d r).
$$

Sea $P_b$ el promedio adaptativo por canal en una rejilla de $b\times b$. Las cinco ramas aportan 16 canales cada una. La rama global promedia todo el mapa.

$$
R_d=\rho(C_{3,d}(H)),\quad d\in\{2,4,8\},\qquad
G=U_{64}(\rho(C_1(P_1(H)))).
$$

$$
Q=[\rho(C_1(H)),R_2,R_4,R_8,G],\qquad
z=U_{256}\big(C_1^{32\to1}(\rho(C_1^{80\to32}(Q)))\big).
$$

La dilatación espacia las muestras sin reducir el mapa. El diseño supone que reconocer el rótulo requiere combinar su apariencia local con indicios a varias distancias. La rama global aporta el mismo resumen a todas las posiciones.

### PSPNet compacta · pirámide de promedios regionales

Si $\Omega^{(b)}_{ij}$ es la ventana adaptativa de la celda $(i,j)$, cada rama calcula

$$
(P_b H)_{cij}=\frac{1}{|\Omega^{(b)}_{ij}|}\sum_{u\in\Omega^{(b)}_{ij}} H_c(u),\qquad
R_b=U_{64}(\rho(C_1^{64\to16}(P_b H))),\quad b\in\{1,2,3,6\}.
$$

$$
Q=[H,R_1,R_2,R_3,R_6],\qquad
z=U_{256}\big(C_1^{32\to1}(\rho(C_1^{128\to32}(Q)))\big).
$$

Los 64 canales locales y las cuatro ramas regionales de 16 canales forman la entrada de 128 canales a la cabeza. El diseño supone que la región que rodea al rótulo ayuda a interpretarlo. Si ocupa pocos píxeles, su señal puede diluirse en el promedio regional. La ruta directa de $H$ conserva información local.

Las cinco arquitecturas aprenden mediante la misma pérdida BCE–Dice y el mismo procedimiento de Adam. Las expresiones de ambos y su relación con los píxeles válidos se desarrollan a continuación.
