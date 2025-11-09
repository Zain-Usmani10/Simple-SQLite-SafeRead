Welcome to this SQLite proof-of-concept, which demonstrates how the SQLite Online Backup API is used to create an export a snapshot database.
- This compiled source code creates reader and writer applications, which actively write data to a file every second.
- Running the reader application will automatically create a snapshot database along with a corresponding csv file of the same name.
- The csv file is meant to be exported out via a USB stick for data analysis and visualization.

This demonstration is done for the implementation of an SQLite DB on the UWindsor Formula Electric Racecar.
This system will be applied and stored on an onboard Rasberry Pi which is always collecting data from various sensors in the car.

Ultimately, this data will help us analyze the car's performance, and allow us to visualize multiple metrics such as speed, acceleration, heat, braking power, etc.
