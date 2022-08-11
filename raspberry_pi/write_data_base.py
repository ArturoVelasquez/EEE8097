#TODO get id for module and match them before pushing to db
import csv
import pandas as pd
import json
from sqlalchemy import create_engine

filename="test.csv"
table="measuremenets"
secrets="credentials.json"

def read_register():
    measurements = pd.read_csv(filename)
    return measurements

def resert_register():
    with open(filename, 'w',newline='') as file:
        typer=csv.writer(file)
        typer.writerow(["date_time","module_number","module_type","temperature",'humidity',"soil_moisture","light_intesity","valve_satatus","nitrogen","phohorus","potasium","latitude","longitune"])
        file.close()

def push_to_db(measurements):
    with open(secrets,'r') as file:
        credentials=json.load(file)
        file.close()
    
    db_url=f"{credentials['engine']}://{credentials['username']}:{credentials['password']}@{credentials['host']}:5432/{credentials['db_name']}"
    print(db_url)
    
    engine = create_engine(db_url)
    measurements.to_sql(table, con=engine, if_exists='append',index=False, chunksize=1000,method='multi')
