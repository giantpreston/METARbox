from flask import Flask, request, Response
import requests

app = Flask(__name__)

@app.route("/metar/<station_code>")
def metar_proxy(station_code):
    url = f"https://tgftp.nws.noaa.gov/data/observations/metar/stations/{station_code.upper()}.TXT" # Get the metar from here, basically proxying it since the ESP struggles with HTTPS through AT.
    try:
        response = requests.get(url)
        return Response(response.text, content_type='text/plain')
    except Exception as e:
        return Response(f"Error: {str(e)}", status=500)

if __name__ == "__main__":
    # Using host="0.0.0.0" to expose the server
    app.run(host="0.0.0.0", port=3000)
    # Change the port here if you want to!
