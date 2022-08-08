import json
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pyparsing import stringStart
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

def get_info(connection, query, date_columns=None):
    info=pd.read_sql_query(query,con=connection,parse_dates=date_columns)
    return info

def generate_test_data(start=[10.61576,-75.05048],stop=[10.61684,-75.04792],samples=100):

    latitude = (stop[0]-start[0])*np.random.random_sample(samples)+start[0]
    longitude = (stop[1]-start[1])*np.random.random_sample(samples)+start[1]

    temperature=np.random.random_sample(100)*40
    humidity=np.random.random_sample(100)*100
    light_intensity=np.random.random_sample(100)*100
    soil_moisture=np.random.random_sample(100)*100
    valve_status=np.random.randint(5,size=100)

    static = pd.DataFrame(np.column_stack((temperature,humidity,light_intensity,soil_moisture,valve_status,latitude,longitude)),columns=['temperature','humidity','light_intesity','soil_moisture','valve_status','latitude','longitude'])
    
    nitrogen=np.random.random_sample(100)*100
    phosphorus=np.random.random_sample(100)*100
    potasium=np.random.random_sample(100)*100

    latitude = (stop[0]-start[0])*np.random.random_sample(samples)+start[0]
    longitude = (stop[1]-start[1])*np.random.random_sample(samples)+start[1]

    mobile = pd.DataFrame(np.column_stack((nitrogen,phosphorus,potasium,latitude,longitude)),columns=['nitrogen','phosphorus','potasium','latitude','longitude'])
    return static, mobile

def plot_satatic(data, ht=True,ls=True,v=True):
    return ht



def plot_mobile(data,nitrogen=True,phosphorus=True,potasium=True):
    lon=data['longitude']
    lat=data['latitude']
    med_lat=lat.median()
    med_lon=lon.median()
    max_size=data[['nitrogen', 'phosphorus','potasium']].max(axis=1).max().round(2)
    min_size=data[['nitrogen', 'phosphorus','potasium']].min(axis=1).min().round(2)
    med_size=data[['nitrogen', 'phosphorus','potasium']].median(axis=1).median().round(2)
    fig,ax = plt.subplots(figsize=(10, 10))
    mobile_map = Basemap(projection='tmerc', resolution='l',lat_0=med_lat, lon_0=med_lon,llcrnrlon=lon.min()-0.0009,llcrnrlat=lat.min()-0.0003,urcrnrlon=lon.max()+0.0004,urcrnrlat=lat.max()+0.0003)
    mobile_map.drawcoastlines(color='gray')
    mobile_map.drawrivers(color='#DDEEFF')
    mobile_map.drawmapboundary(fill_color="#DDEEFF")
    mobile_map.fillcontinents(color="#FFFFFF", lake_color='#DDEEFF')
    mobile_map.drawparallels(np.arange(lat.min(),lat.max(),0.0005),labels=[0,1,0,0],xoffset=15) # draw parallels
    mobile_map.drawmeridians(np.arange(lon.min(),lon.max(),0.0005),labels=[0,0,0,1],yoffset=15) # draw meridians
    
    position=mobile_map.scatter(lon,lat,marker='x',color='k',latlon=True, label='Coordinates')
    legend0=plt.legend(handles=[position],scatterpoints=1, frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='center left')
    ax.add_artist(legend0)
    
    
    if nitrogen:
        n_plot=mobile_map.scatter(lon, lat, latlon=True, c='#8F8FFF', s=data['nitrogen']*5, alpha=0.5,label="Nitrogen")

    if phosphorus:
        ph_plot=mobile_map.scatter(lon, lat, latlon=True, c='#FFA500', s=data['phosphorus']*5, alpha=0.5,label="Phosphorus")

    if potasium:
        p_plot=mobile_map.scatter(lon, lat, latlon=True, c='#38664c', s=data['potasium']*5, alpha=0.5,label="Potasium")

    mobile_map.scatter(lon,lat,marker='x',color='k',latlon=True,alpha=0.5)
    legend1=plt.legend(handles=[n_plot,ph_plot,p_plot],scatterpoints=1, frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='lower left',title="Nutrient Type")
    ax.add_artist(legend1)
    
    plt.title("Soil Nutrients")
    plt.xlabel("longitude")
    ax.yaxis.set_label_position("right")
    plt.ylabel("latitude")
    
    small=plt.scatter([], [], c='k', alpha=0.5, s=min_size*5,label=str(min_size))
    medium=plt.scatter([], [], c='k', alpha=0.5, s=med_size*5,label=str(med_size))
    large=plt.scatter([], [], c='k', alpha=0.5, s=max_size*5,label=str(max_size))
    
    plt.legend(handles=[small,medium,large],frameon=True, framealpha=1, borderpad=1.2,labelspacing=1, loc='upper left',title="Nutrient density");

    plt.show()
    plt.savefig('nutrients.png')

data=generate_test_data()
plot_mobile(data[1])

# # 1. Draw the map background
# fig = plt.figure(figsize=(8, 8))
# m = Basemap(projection='tmerc', resolution='h', 
#             lat_0=avg_lat, lon_0=avg_lon,
#             #width=0.5E3, height=0.6E3)
#             llcrnrlon=lon.min()-0.0005,llcrnrlat=lat.min()-0.001,
#             urcrnrlon=lon.max()+0.0005,urcrnrlat=lat.max()+0.001)
# #m.bluemarble()
# m.drawcoastlines(color='gray')
# #m.drawcountries(color='gray')
# #m.drawstates(color='gray')
# m.drawmapboundary(fill_color="#DDEEFF")
# m.fillcontinents(color="#c2debd", lake_color='#DDEEFF')

# # 2. scatter city data, with color reflecting population
# # and size reflecting area
# m.scatter(lon,lat,marker='x',color='k',latlon=True)#, alpha=0.5)
# m.scatter(lon, lat, latlon=True,c=temperature, s=humidity*5,cmap='Reds', alpha=0.5)

# # 3. create colorbar and legend
# plt.colorbar(label='Temperature C')
# plt.clim(temperature.min(), temperature.max())

# # make legend with dummy points
# for a in ["humidity",humidity.min().round(2), humidity.median().round(2), humidity.max().round(2)]:
#     if type(a)==str:
#         plt.scatter([], [], c='k', alpha=0.5,s=0,
#                     label=a )
#     else:
#         plt.scatter([], [], c='k', alpha=0.5, s=a,
#                     label=str(a) + '%')
# plt.legend(scatterpoints=1, frameon=False,
#            labelspacing=1, loc='lower left');