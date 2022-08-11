import json
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime
from sqlalchemy import create_engine
from mpl_toolkits.basemap import Basemap

filename="test.csv"
table="measuremenets"
secrets="credentials.json"

# This method generates a db connection when given a json file with the required info
def create_connetion(secrets_file=secrets):
    with open(secrets_file, 'r') as file:
        credentials=json.load(file)
        file.close()
    
    db_url=f"{credentials['engine']}://{credentials['username']}:{credentials['password']}@{credentials['host']}:5432/{credentials['db_name']}"
    print(db_url)

    engine = create_engine(db_url)
    return engine 


#This method formats the queries with the require date 
def format_queries(date1='2020-08-01',date2=datetime.now(),dquery=None):
    queries=json.load(open('server/queries.json'))

    if dquery=='static':
        query=queries['static_data'].format(date1,date2)
    if dquery=='mobile':
        query=queries['mobile_data'].format(date1,date2)
    return query

# This method collects the data needed to make the plots
def get_info(connection, query, date_columns=None):
    info=pd.read_sql_query(query,con=connection,parse_dates=date_columns)
    return info

#This methos generates fake data to test the plots
def generate_test_data(start=[10.61576,-75.05048],stop=[10.61684,-75.04792],samples=100):

    latitude = (stop[0]-start[0])*np.random.random_sample(samples)+start[0]
    longitude = (stop[1]-start[1])*np.random.random_sample(samples)+start[1]

    temperature=np.random.random_sample(100)*40
    humidity=np.random.random_sample(100)*100
    light_intensity=np.random.random_sample(100)*100
    soil_moisture=np.random.random_sample(100)*100
    valve_status=np.random.randint(5,size=100)

    static = pd.DataFrame(np.column_stack((temperature,humidity,light_intensity,soil_moisture,valve_status,latitude,longitude)),columns=['temperature','humidity','light_intensity','soil_moisture','valve_status','latitude','longitude'])
    
    # Expected range values for Nitrogen is from 0-60mg/kg, and for Phosphosrus and Potasim is 0-300 mg/kg
    nitrogen=np.random.random_sample(100)*60
    phosphorus=np.random.random_sample(100)*300
    potasium=np.random.random_sample(100)*300

    latitude = (stop[0]-start[0])*np.random.random_sample(samples)+start[0]
    longitude = (stop[1]-start[1])*np.random.random_sample(samples)+start[1]

    mobile = pd.DataFrame(np.column_stack((nitrogen,phosphorus,potasium,latitude,longitude)),columns=['nitrogen','phosphorus','potasium','latitude','longitude'])
    return static, mobile

#This method genrates the plot for the soil nutrientes.
def plot_mobile(data,nitrogen=True,phosphorus=True,potasium=True,name='nutrients'):
    lon=data['longitude']
    lat=data['latitude']
    
    med_lat=lat.median()
    med_lon=lon.median()
    max_size=data[['nitrogen', 'phosphorus','potasium']].max(axis=1).max().round(2)
    min_size=data[['nitrogen', 'phosphorus','potasium']].min(axis=1).min().round(2)
    med_size=data[['nitrogen', 'phosphorus','potasium']].median(axis=1).median().round(2)
    
    fig,ax = plt.subplots(figsize=(20,8))
    mobile_map = Basemap(projection='tmerc', resolution='l',lat_0=med_lat, lon_0=med_lon,llcrnrlon=lon.min()-0.0008,llcrnrlat=lat.min()-0.0003,urcrnrlon=lon.max()+0.0006,urcrnrlat=lat.max()+0.0003)
    mobile_map.drawcoastlines(color='gray')
    mobile_map.drawrivers(color='#DDEEFF')
    mobile_map.drawmapboundary(fill_color="#DDEEFF")
    mobile_map.fillcontinents(color="#FFFFFF", lake_color='#DDEEFF')
    mobile_map.drawparallels(np.arange(lat.min(),lat.max(),0.0002),labels=[0,1,0,0],xoffset=15) # draw parallels
    mobile_map.drawmeridians(np.arange(lon.min(),lon.max(),0.0005),labels=[0,0,0,1],yoffset=15) # draw meridians
    
    position=mobile_map.scatter(lon,lat,marker='x',color='k',latlon=True, label='Global position')
    legend0=plt.legend(handles=[position],scatterpoints=1, frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='center left')
    ax.add_artist(legend0)
    
    if nitrogen:
        n_plot=mobile_map.scatter(lon, lat, latlon=True, c='#8F8FFF', s=data['nitrogen']*4, alpha=0.5,label="Nitrogen")
    else:
        n_plot=plt.scatter([], [])
    if phosphorus:
        ph_plot=mobile_map.scatter(lon, lat, latlon=True, c='#FFA500', s=data['phosphorus']*4, alpha=0.5,label="Phosphorus")
    else:
        ph_plot=plt.scatter([], [])
    if potasium:
        p_plot=mobile_map.scatter(lon, lat, latlon=True, c='#38664c', s=data['potasium']*4, alpha=0.5,label="Potasium")
    else:
        p_plot=plt.scatter([], [])
 
    legend1=plt.legend(handles=[n_plot,ph_plot,p_plot],scatterpoints=1, frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='lower left',title="Nutrient Type")
    ax.add_artist(legend1)
    
    plt.title("Soil Nutrients")
    plt.xlabel("longitude")
    ax.yaxis.set_label_position("right")
    plt.ylabel("latitude")
    
    small=plt.scatter([], [], c='k', alpha=0.5, s=min_size*2,label=str(min_size))
    medium=plt.scatter([], [], c='k', alpha=0.5, s=med_size*2,label=str(med_size))
    large=plt.scatter([], [], c='k', alpha=0.5, s=max_size*2,label=str(max_size))
    
    plt.legend(handles=[small,medium,large],frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='upper left',title="Nutrient density mg/kg");

    plt.show()
    plt.savefig(f'{name}.png')

#this method generates the plots for the ambient nutrientes
def plot_static(data, ht=True,ls=True,v=True,name='values'):
    lon=data['longitude']
    lat=data['latitude']
    med_lat=lat.median()
    med_lon=lon.median()
    
    max_size_ht = data[['temperature','humidity']].max(axis=1).max().round(2)
    min_size_ht = data[['temperature','humidity']].min(axis=1).min().round(2)
    med_size_ht = data[['temperature','humidity']].median(axis=1).median().round(2)

    max_size_ls = data[['light_intensity','soil_moisture']].max(axis=1).max().round(2)
    min_size_ls = data[['light_intensity','soil_moisture']].min(axis=1).min().round(2)
    med_size_ls = data[['light_intensity','soil_moisture']].median(axis=1).median().round(2)

    fig,ax = plt.subplots(figsize=(20, 8))
    mobile_map = Basemap(projection='tmerc', resolution='l',lat_0=med_lat, lon_0=med_lon,llcrnrlon=lon.min()-0.0006,llcrnrlat=lat.min()-0.0003,urcrnrlon=lon.max()+0.0006,urcrnrlat=lat.max()+0.0003)
    mobile_map.drawcoastlines(color='gray')
    mobile_map.drawrivers(color='#DDEEFF')
    mobile_map.drawmapboundary(fill_color="#DDEEFF")
    mobile_map.fillcontinents(color="#FFFFFF", lake_color='#DDEEFF')
    mobile_map.drawparallels(np.arange(lat.min(),lat.max(),0.0002),labels=[0,1,0,0],xoffset=15) # draw parallels
    mobile_map.drawmeridians(np.arange(lon.min(),lon.max(),0.0005),labels=[0,0,0,1],yoffset=15) # draw meridians
    

    if ht:
        mobile_map.scatter(lon, lat, latlon=True, c=data['temperature'],cmap='seismic', s=data['humidity']*10,alpha=0.7,edgecolor='k')
        plt.colorbar(label='Temperature',location="left")
        
        small = plt.scatter([], [], c='k', alpha=0.5, s=min_size_ht*2,label=str(min_size_ht))
        medium = plt.scatter([], [], c='k', alpha=0.5, s=med_size_ht*2,label=str(med_size_ht))
        large = plt.scatter([], [], c='k', alpha=0.5, s=max_size_ht*2,label=str(max_size_ht))
        
        legend0 = plt.legend(handles=[small,medium,large],frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='center left',title="Humidity %");
        ax.add_artist(legend0)
    if ls:
        mobile_map.scatter(lon, lat, latlon=True, c=data['light_intensity'],cmap='gnuplot2', s=data['soil_moisture']*10,alpha=0.7,edgecolor='k')
        plt.colorbar(label='Light intensity',location="left")
        
        small = plt.scatter([], [], c='k', alpha=0.5, s=min_size_ls*2,label=str(min_size_ls))
        medium = plt.scatter([], [], c='k', alpha=0.5, s=med_size_ls*2,label=str(med_size_ls))
        large = plt.scatter([], [], c='k', alpha=0.5, s=max_size_ls*2,label=str(max_size_ls))
        legend0 = plt.legend(handles=[small,medium,large],frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='center left',title="Soil moisture");
        ax.add_artist(legend0)
   
    if v:
        mobile_map.scatter(lon, lat, latlon=True, c=data['valve_status'],cmap="tab10",s=500,alpha=0.7,edgecolor='k')
        plt.colorbar(label='Valve status',location="left")
        small = plt.scatter([], [])
        medium = plt.scatter([], [])
        large = plt.scatter([], [])
        
    position=mobile_map.scatter(lon,lat,marker='x',color='k',latlon=True, label='Global position',alpha=0.7)
    legend1=plt.legend(handles=[position],scatterpoints=1, frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='lower left')
    ax.add_artist(legend1)
    
    plt.title("Field conditions")
    plt.xlabel("longitude")
    ax.yaxis.set_label_position("right")
    plt.ylabel("latitude")
    
    plt.show()
    plt.savefig(F'{name}.png')
