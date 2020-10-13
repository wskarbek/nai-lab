import numpy as np
from cv2 import cv2

cap = cv2.VideoCapture(0)

print("Video size: ", cap.get(cv2.CAP_PROP_FRAME_WIDTH) , "x" , cap.get(cv2.CAP_PROP_FRAME_WIDTH))
while(True):
    ret, frame = cap.read()
    frame = cv2.flip(frame, 1)
    cv2.imshow("Not-yet smart windown", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()