import sys
import cv2
from time import time

seconds = 0
fps = 0
max_fps = 0
avg_fps = 0
frame_count = 0

if len(sys.argv) > 1:
    argument = sys.argv[1]
else:
    argument = 'hw2'

if argument == 'no-hw' or  argument== 'no-hardware-accelerate':
    #OpenCV with CUDA but no HW decoder.
    cap = cv2.VideoCapture('demo.mp4')
elif argument == 'hw1' or  argument == 'hardware-accelerate1':
    #Using gstream to accelerate the decoding process
    cap = cv2.VideoCapture('filesrc location=demo.mp4 ! qtdemux ! queue ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,format=BGRx ! queue ! videoconvert ! queue ! video/x-raw, format=BGR ! appsink', cv2.CAP_GSTREAMER)
else:
    #Using gstream to accelerate the decoding process but resize in Gstreamer
    cap = cv2.VideoCapture('filesrc location=demo.mp4 ! qtdemux ! queue ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,format=BGRx,width=1280,height=720 ! queue ! videoconvert ! queue ! video/x-raw, format=BGR ! appsink', cv2.CAP_GSTREAMER)

font = cv2.FONT_HERSHEY_SIMPLEX
start_avg = float(time())

while(True):

    #Start the timer for FPS calculation
    start = float(time())

    # Capture frame-by-frame
    ret, frame = cap.read()

    # Create the string for displaying FPS
    fps_str = "FPS:"
    fps_str += "{:.2f}".format(fps)

    max_fps_str = "Max FPS:"
    max_fps_str += "{:.2f}".format(max_fps)

    avg_fps_str = "Avg FPS:"
    avg_fps_str += "{:.2f}".format(avg_fps)
    frame_count += 1

    if argument == 'no-hw' or argument == 'no-hardware-accelerate' or argument == 'hw1' or  argument == 'hardware-accelerate1':
        #Resize the image
        New=cv2.resize(frame,(1280,720),fx=0,fy=0, interpolation = cv2.INTER_CUBIC)
        cv2.putText(New , fps_str , (10,640), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.putText(New , max_fps_str , (10,670), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.putText(New , avg_fps_str , (10,700), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.imshow('frame',New)
    else:
        cv2.putText(frame , fps_str , (10,640), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.putText(frame , max_fps_str , (10,670), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.putText(frame , avg_fps_str , (10,700), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.imshow('frame',frame)

    # Stop the timer and calcuate the FPS
    end = float(time())
    fps = 1/(end - start)
    #print("Estimated frames per second : {0}".format(fps))


    if fps > max_fps:
        max_fps = fps

    avg_fps = (avg_fps + fps) / 2

    #Stop if user press q key
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

stop_avg = float(time())
avg_fps = frame_count / (stop_avg - start_avg)

# When everything done, release the capture

print(avg_fps)
cap.release()
cv2.destroyAllWindows()