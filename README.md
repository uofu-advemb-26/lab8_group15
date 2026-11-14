# Lab 8 Results
## Activity 0
**Transmission Side**: ![Activity 0 TX](/images/activity0_tx.png "Activity 0 TX")
  - Sending message 'CANbus' every 1 second

**Reception Side**: ![Activity 0 RX](/images/activity0_rx.png "Activity 0 RX")
  - Received the 'CANbus' message every 1 second after it was transmitted

**Oscilloscope**: ![Activity 0 Oscilloscope](/images/activity0_scope.png "Activity 0 Oscilloscope")
  - **Key**
    - Probe 1 is CAN High
    - Probe 2 is CAN Low
    - Probe 3 is microcontroller TX
  - **Observations**
    - The microcontroller outputs 0V and 3.3V signals.
    - On the CAN Bus, 0s are translated to a differential signal where CAN Low is pulled to 1.5V and CAN High is pulled to 3.5V.
    - 1s are translated to 2.5V on both lines.

## Activity 1

**Oscilloscope**:
![Activity 1 Oscilloscope](/images/activity1_scope.png "Activity 1 Oscilloscope")
  - **Key**
    - Probe 1 is CAN High
    - Probe 2 is CAN Low
    - Probe 3 is microcontroller TX

### Continuous High Priority Transmission
**Observations**
  - When the high priority message is set to broadcast continuously, the lower priority message never had a chance to be transmitted. The low priority side occasionally believed it had transmitted, but this message was never observed on the high priority side or on the oscilloscope.

**High Priority Side**:
![Activity 1 High Priority (No Delay)](/images/activity1_high_no_delay.png "Activity 1 High Priority (No Delay)")

**Low-Priority Side**:
![Activity 1 Low Priority (No Delay)](/images/activity1_low_no_delay.png "Activity 1 Low Priority (No Delay)")

### 10ms High Priority Transmission
**Observations**
  - With a 180us to ~1ms delay between high priority transmissions, the start of the low priority message was observed on the oscilloscope, but the high priority side only reported a CAN reception error.
  - It wasn't until about 10ms between high priority transmissions that the low priority transmission was able to occasionally get through fully. This was periodically (but too briefly to capture) shown on the oscilloscope, but it is shown in the terminal outputs below.

**High Priority Side**:
![Activity 1 High Priority (10ms Delay)](/images/activity1_high_10ms.png "Activity 1 High Priority (10ms Delay)")

**Low-Priority Side**:
![Activity 1 Low Priority (10ms Delay)](/images/activity1_low_10ms.png "Activity 1 Low Priority (10ms Delay)")