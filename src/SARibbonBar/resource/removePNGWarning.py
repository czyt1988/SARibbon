import os
from tqdm import tqdm
import cv2
from skimage import io
path = r"./"

fileList = os.listdir(path)
for i in tqdm(fileList):
    image = io.imread(path+i)
    image = cv2.cvtColor(image, cv2.COLOR_RGBA2BGRA)
    cv2.imencode('.png',image)[1].tofile(path+i)

