import os
from tqdm import tqdm
import cv2
from skimage import io
path = r"./"
#西瓜6的代码
fileList = os.listdir(path)
for i in tqdm(fileList):
    image = io.imread(path+i)
    image = cv2.cvtColor(image, cv2.COLOR_RGBA2BGRA)
    cv2.imencode('.png',image)[1].tofile(path+i)

convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/ArrowUp.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/ArrowUp.png
convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/ribbonPannelOptionButton.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/ribbonPannelOptionButton.png
convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Close.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Close.png
convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Close_Hover.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Close_Hover.png
convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Max.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Max.png
convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Min.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Min.png
convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Normal.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Normal.png
convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Shade.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/Titlebar_Shade.png
convert.exe E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/ribbonPannelOptionButton.png E:/cloudDisk/Qt/sa/src/SARibbonBar/SARibbon/src/SARibbonBar/resource/ribbonPannelOptionButton.png


QSyntaxHighlighter 或者第三方控件Qscitinlla（https://www.cnblogs.com/pinking/p/7788447.html）