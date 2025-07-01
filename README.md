# Line Follower Robot with Obstacle Detection

An autonomous **line follower robot** enhanced with **obstacle detection** functionality. The project combines mechanics, electronics, and programming to build a mobile robot that can follow a predefined path and intelligently avoid obstacles using ultrasonic sensors.

![image](https://github.com/user-attachments/assets/edd7f536-0696-4141-975a-28b833664fc4)



---

## 🛠️ Mechanical Design

The mechanical structure was fully designed in SolidWorks. Key components include:

- **Base and cover** – 3D printed in PLA
- **Drive system** – 2× N20-BT38 micro motors (250:1, 120RPM)
- **Wheels** – Pololu 1087 (32×7 mm)
- **Caster ball** – Pololu 2691 ball caster
- **Ultrasonic sensor mount** – transparent acrylic bracket

![image](https://github.com/user-attachments/assets/8357d3df-13af-4c6f-9fa3-8936640cf543)
---

## 🔌 Electronics

The electronic system includes:

- **Arduino Uno Rev3**
- **L298N dual H-bridge motor driver**
- **HC-SR04 ultrasonic distance sensor**
- **TCRT5000 LM393 IR reflective sensor**
- **SG-90 servo motor**
- **18650 Li-ion battery**
- **Rocker switch**

![image](https://github.com/user-attachments/assets/0217f568-6a9e-4cc2-bfbb-bd53b7781fd0)

![image](https://github.com/user-attachments/assets/56d02b74-b7d0-475d-8403-6fdfce4c386a)

![image](https://github.com/user-attachments/assets/fe9b71ff-b248-482d-93f8-3f24822c14f7)

![image](https://github.com/user-attachments/assets/ac5b681b-22c7-4a10-a737-eb58a379cc0c)

![image](https://github.com/user-attachments/assets/b96585f4-5fcf-48d4-8c8c-289093d325ca)

### 🧠 Algorithm Description

The robot uses a **hybrid control algorithm** that combines **line following** and **obstacle avoidance**:

#### 🚗 Line Following

- IR sensors underneath detect a black line on a light background.
- Movement logic:
  - Left sensor sees black → slight left turn
  - Right sensor sees black → slight right turn
  - Both see black → go forward
  - Both see white → stop or reverse and re-center

#### 🚧 Obstacle Avoidance

- The ultrasonic sensor scans ahead using a mounted servo.
- If an object is closer than a threshold (e.g., 20 cm), the robot:
  1. Stops
  2. Rotates the servo to scan left/right
  3. Chooses the direction with more space
  4. Turns accordingly
  5. Re-centers and resumes line tracking

#### 🔄 Switching Logic

| Condition                   | Action                               |
|-----------------------------|---------------------------------------|
| Line detected, no obstacle  | Follow the line                      |
| Obstacle detected           | Pause line tracking → avoid obstacle |
| Obstacle cleared            | Resume line tracking                 |

---

## 🧪 Features & Testing

- Obstacle detection range: 2–200 cm
- Collision avoidance logic
- Adjustable IR sensor sensitivity
- Track-following: straights, turns, and obstacles
- Speed tuning and calibration

![image](https://github.com/user-attachments/assets/460b5e80-4488-4da3-8a91-1a9da763f8f0)


---

## 📄 Technical Documentation

📎 [Download Full Report (PDF)](https://github.com/Bajtii/Line-follower-with-obstacle-avoiding/blob/main/Linefollower_documentation.pdf)  
Includes hardware schematics, control algorithm explanation, sensor placement, and performance test results.

---

## 👨‍🔧 Author

**Bartosz Wojdon**  
Mechatronics Engineering, AGH University of Science and Technology  
Academic Year 2024/2025  


