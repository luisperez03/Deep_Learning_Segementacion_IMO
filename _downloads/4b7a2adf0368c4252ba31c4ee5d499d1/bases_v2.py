"""Adaptaciones compactas para segmentación binaria, sin pesos externos."""
import torch
from torch import nn
from torch.nn import functional as F
from modelos_base import block

class Piramide(nn.Module):
    def __init__(self, modo):
        super().__init__()
        self.encoder = nn.Sequential(block(3,16), nn.MaxPool2d(2), block(16,32),
                                     nn.MaxPool2d(2), block(32,64))
        self.modo = modo
        if modo == 'aspp':
            self.ramas = nn.ModuleList([nn.Conv2d(64,16,1)] + [
                nn.Conv2d(64,16,3,padding=d,dilation=d) for d in (2,4,8)] +
                [nn.Sequential(nn.AdaptiveAvgPool2d(1),nn.Conv2d(64,16,1))])
            canales = 80
        else:
            self.ramas = nn.ModuleList([nn.Sequential(nn.AdaptiveAvgPool2d(b),
                           nn.Conv2d(64,16,1)) for b in (1,2,3,6)])
            canales = 128
        self.head = nn.Sequential(nn.Conv2d(canales,32,1),nn.ReLU(),nn.Conv2d(32,1,1))
    def forward(self,x):
        h = self.encoder(x)
        partes = [h] if self.modo == 'psp' else []
        for rama in self.ramas:
            y = F.relu(rama(h))
            if y.shape[-2:] != h.shape[-2:]:
                y = F.interpolate(y,size=h.shape[-2:],mode='bilinear',align_corners=False)
            partes.append(y)
        return F.interpolate(self.head(torch.cat(partes,1)),size=x.shape[-2:],
                             mode='bilinear',align_corners=False)
BASES = {'deeplab_compacta': lambda: Piramide('aspp'), 'pspnet_compacta': lambda: Piramide('psp')}

