from fastapi import FastAPI
import server.generate_maps as gs
from fastapi.responses import StreamingResponse

app =FastAPI()

data=gs.generate_test_data()

@app.get("/")
async def root():
    return {"message":"This will be my maps API"}

@app.get("/soil")
def soil(N:bool=True,P:bool=True,K:bool=True,name:str="nutrients"):
    gs.plot_mobile(data[1],N,P,K,name)
    def iterfile():
        with open(f'{name}.png', mode="rb") as file_like:  # 
                yield from file_like  # 

    return StreamingResponse(iterfile(), media_type="image/png")

#plot_static(data, ht=True,ls=True,v=True,name='values')
@app.get("/weather")
def wetaher(ht:bool=True,ls:bool=False,v:bool=False,name:str="weather"):
    gs.plot_static(data[0],ht,ls,v,name)
    def iterfile():
        with open(f'{name}.png', mode="rb") as file_like:  # 
                yield from file_like  # 

    return StreamingResponse(iterfile(), media_type="image/png")