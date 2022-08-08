import csv
from datetime import datetime
import pandas as pd

filename="test.csv"
measured_time= datetime.now().strftime("%Y-%m-%d %H:%M:%S")

values_gps=(0x02,'0','4',15,30,40,15.151163681,13.1357171181)

values_static=(0x01,'0','1',42.371163681,53.1357171181,30,45,4)

with open(filename, 'w',newline='') as file:
    typer=csv.writer(file)
    typer.writerow(["date_time","module_number","module_type","temperature",'humidity',"soil_moisture","light_intesity","valve_satatus","nitrogen","phohorus","potasium","latitude","longitune"])
    file.close()

with open(filename, 'a', newline='') as file:
    typer= csv.writer(file)
    typer.writerow([measured_time,values_static[0],values_static[1]+values_static[2],round(values_static[3],2),round(values_static[4],2),values_static[5],values_static[6],values_static[7],'','','','',''])
    file.close()

with open(filename, 'a', newline='') as file:
    typer= csv.writer(file)
    typer.writerow([measured_time,values_gps[0],values_gps[1]+values_gps[2],'','','','','',values_gps[3],values_gps[4],values_gps[5],round(values_gps[6],5),round(values_gps[7],5)])
    file.close()

my_data=pd.read_csv(filename)

my_data.head()

