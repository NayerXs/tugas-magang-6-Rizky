import rospy
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
import time

def main():
    rospy.init_node('Video_Publish', anonymous=True)
    pub = rospy.Publisher('/camera_frame', Image, queue_size=10)
    bridge = CvBridge()
    cap = cv2.VideoCapture(0)
    cap.set(3, 640)
    cap.set(4, 480)
    prev_time = time.time()

    while not rospy.is_shutdown():
        ret, frame = cap.read()
        if not ret:
            continue

        frame = cv2.flip(frame, 1)
        curr_time = time.time()
        fps = 1.0 / (curr_time - prev_time)
        prev_time = curr_time
        cv2.putText(frame, f'FPS: {fps:.2f}', (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

        cv2.imshow("Camera Publisher", frame)
        if cv2.waitKey(1) & 0xFF == ord('s'):
            cv2.imwrite('saved_frame.jpg', frame)
            rospy.loginfo("Frame saved!")

        pub.publish(bridge.cv2_to_imgmsg(frame, "bgr8"))

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()
