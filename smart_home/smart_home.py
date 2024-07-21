from flask import Flask, request, redirect, url_for, send_from_directory
import os
from unihiker import GUI   # Import the package
from dataclasses import dataclass 
import time
from pinpong.board import Board, Pin, Tone  # Import the Board, Pin, and Tone modules from the pinpong.board package

debug = True
gui = GUI()  # Instantiate the GUI class

@dataclass
class button:
    name:str     #button name
    state: bool  #button state
    image: gui   #button image
    cor: list    #coordinate of the button

        

def switch_clicked(button):
    gui.remove(button.image)
    if button.state:
        button.image= gui.draw_image(x=button.cor[0], y=button.cor[1], w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button))
    else:
        button.image= gui.draw_image(x=button.cor[0], y=button.cor[1], w=50, h=50, image='/root/my_codes/smart_home/images/power-button-on.png', onclick=lambda: switch_clicked(button))
    button.state=not button.state
    if debug:
        print(button.name,button.state)

Board().begin()  # Initialize the board, choose the board type and port number (auto-detection if not specified)

tone = Tone(Pin(Pin.P26))  # Create a Tone object with Pin.P26 for analog output
tone.freq(200)  # Set the frequency to 200 for the tone playback

button1 = button(name="button1",state=0,image=gui.draw_image(x=5, y=50, w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button1)),cor=[5,50]) 
button2 = button(name="button2", state=0, image=gui.draw_image(x=60, y=50, w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button2)),cor=[60,50])
button3 = button(name="button3", state=0, image=gui.draw_image(x=115, y=50, w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button3)),cor=[115,50])
button4 = button(name="button4", state=0, image=gui.draw_image(x=170, y=50, w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button4)),cor=[170,50])
button5 = button(name="button5", state=0, image=gui.draw_image(x=5, y=105, w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button5)),cor=[5,105])
button6 = button(name="button6", state=0, image=gui.draw_image(x=60, y=105, w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button6)),cor=[60,105])
button7 = button(name="button7", state=0, image=gui.draw_image(x=115, y=105, w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button7)),cor=[115,105])
button8 = button(name="button8", state=0, image=gui.draw_image(x=170, y=105, w=50, h=50, image='/root/my_codes/smart_home/images/power-button-off.png', onclick=lambda: switch_clicked(button8)),cor=[170,105])

#image of person at doorbell
door_bell= gui.draw_image(x=30, y=170, w=180, h=180, image='/root/my_codes/smart_home/images/person.jpg')
door_cam_text =gui.draw_text(text="Door bell",origin="center",x=130,y=300,color="#0066CC")

app = Flask(__name__)

# Define the folder to store uploaded files
UPLOAD_FOLDER = 'uploads'
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)

app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024  # Max upload size is 16MB

@app.route('/')
def index():
    return '''
    <!doctype html>
    <title>Upload File</title>
    <h1>Upload a file</h1>
    <form action="/upload" method=post enctype=multipart/form-data>
      <input type=file name=file>
      <input type=submit value=Upload>
    </form>
    '''

@app.route('/upload', methods=['POST'])
def upload_file():
    global door_bell
    if 'file' not in request.files:
        return 'No file part', 400

    file = request.files['file']
    if file.filename == '':
        return 'No selected file', 400

    if file:
        filename = file.filename
        filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
        gui.remove(door_bell)
        file.save(filepath)
        door_bell= gui.draw_image(x=50, y=170, w=150, h=150, image=filepath)
        tone.on()  # Turn on the tone output
        time.sleep(1.5)  # Delay for 1.5 seconds
        tone.off()  # Turn off the tone output
        return f'File uploaded successfully: {filename}', 200

@app.route('/uploads/<filename>')
def uploaded_file(filename):
    return send_from_directory(app.config['UPLOAD_FOLDER'], filename)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001, debug=True)

while True:
    # Add a delay to prevent the program from exiting and to observe the effects
    time.sleep(1)

# import os

# print(os.getcwd())
