import time,os,sys,asyncio,platform,threading,queue
from tkinter import *
from tkinter import filedialog
from bleak import BleakScanner,BleakClient
from PIL import Image, ImageTk

def resource_path(relative_path):
   base_path = getattr(sys, '_MEIPASS', os.path.dirname(os.path.abspath(__file__)))
   return os.path.join(base_path, relative_path)
update_status = 0
connection_status = False
fwv = ""
bd_address = ""

counter =0
selected_counter =0

blueback_path =  resource_path("Bluebackground.PNG")
image_path =  resource_path("mainim.PNG")
silabs_logo_text_path =  resource_path("Silicon Labs text.PNG")
start_button_path =  resource_path("Start button.PNG")
d1_path =  resource_path("d1.PNG")
d2_path =  resource_path("d2.PNG")
d3_path =  resource_path("d3.PNG")
nearby_path =  resource_path("nearby_pic.PNG")
scan_o_path =  resource_path("scan_o.PNG")
conn_o_path =  resource_path("conn_o.PNG")
conn_wait_path =   resource_path("wait.PNG")
whiteback_path =  resource_path("white_back.PNG")
conn_bt_path =  resource_path("conn_bt.PNG")
fwup_button_path =  resource_path("fw_update_button.PNG")
start_fwup_button_path =  resource_path("start_fw_up_button.PNG")
exit_button_path=resource_path("exit_button.PNG")
icon_path =      resource_path("Silabs icon logo.PNG")
sil_icon_path =  resource_path("Sil icon logo.ICO")

fw_version_uuid = "4F4A2368-8CCA-451E-BFFF-CF0E2EE23E9F"    #current firmware version is read from this UUID
bd_address_uuid = "4B4A2368-8CCA-451E-BFFF-CF0E2EE23E9F"    #BD address is read from this UUID
prelim_info_uuid= "F7BF3564-FB6D-4E53-88A4-5E37E0326063"    #info about number of chunks and Last chunk size is written on this UUID
fw_chunks_uuid  = "984227F3-34FC-4045-A5D0-2C581F81A153"    #firmware chunks are written on this UUID
chunk_size = 224  # size of each firmware chunk to be transferred
BLE_POST_CONNECT_PAUSE_S = 0.5  # settle time before GATT (service discovery / WinRT)
BLE_RECONNECT_DELAY_MS = 2000  # backoff when link drops or connect fails
# Firmware may register FW-version / BD UUIDs with ATT_REC_MAINTAIN_IN_HOST but no GATT read
# callback; centrals that read those attributes can see "Unreachable" and a dropped link on WinRT.
# OTA still works over the write characteristics — set True to skip those reads (recommended for that layout).
OTA_SKIP_FW_VERSION_AND_BD_READS = True

# Bleak WinRT backend must run off the Tk GUI thread — use one dedicated asyncio loop.
_ble_loop_holder = []
_ble_loop_ready = threading.Event()
_ble_gui_queue = queue.Queue()


def _poll_ble_gui_queue():
    try:
        while True:
            on_finish, exc, result = _ble_gui_queue.get_nowait()
            on_finish(exc, result)
    except queue.Empty:
        pass
    root.after(50, _poll_ble_gui_queue)


def _ble_async_worker():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    _ble_loop_holder.append(loop)
    _ble_loop_ready.set()
    loop.run_forever()


def _get_ble_loop():
    if not _ble_loop_holder:
        threading.Thread(target=_ble_async_worker, daemon=True, name="bleak-asyncio").start()
        _ble_loop_ready.wait(timeout=30.0)
    return _ble_loop_holder[0]


def _schedule_on_ble_loop(coro, on_finish):
    """Run awaitable coro on Bleak thread; on_finish(exc, result) runs on Tk main thread."""
    loop = _get_ble_loop()

    def _done(fut):
        try:
            result = fut.result()
            _ble_gui_queue.put((on_finish, None, result))
        except BaseException as e:
            _ble_gui_queue.put((on_finish, e, None))

    fut = asyncio.run_coroutine_threadsafe(coro, loop)
    fut.add_done_callback(_done)


def convert_bytearray_to_string(byte_array):
    result_string = ".".join(map(lambda x: str(x), byte_array[2:10]))
    return result_string
    
def convert_firstbytearray_to_string(first_byte_array):
    first_result_integer = int.from_bytes(first_byte_array[:2], byteorder='little')
    return first_result_integer
    
#start screen animation functions
def anim1():
  global L4
  Start_button["state"] = DISABLED
  L4 = Label(root, image=d1, padx=20, pady=30, borderwidth=0)
  L4.place(x=200, y=158)
  root.after(300,anim2)

def anim2():
  global L5
  L5 = Label(image=d2, padx=20, pady=30, borderwidth=0)
  L5.place(x=160, y=100)
  root.after(300, anim3)
def anim3():
  global L6
  L6 = Label(image=d3, padx=20, pady=30, borderwidth=0)
  L6.place(x=110, y=31)
  root.after(300, bb_2)
def bb_2():
  global blueback_label
  blueback_label = Label(root, image=bb_image, padx=20, pady=30)
  blueback_label.place(x=0, y=0)
  root.after(300, dest_bb)
def dest_bb():
  blueback_label.destroy()
  root.after(500, dest_scan)
def dest_scan():
   previous_labels=[L1,L2,L3,L4,L5,L6]
   for label in previous_labels:
       label.destroy()
   Start_button.destroy()
   root.after(100,start_scan)
def start_scan():
  global scanning_image_label  #l8
  global scanning_text_label
  global name_label
  scanning_image_label = Label(root, image=nearby_image, padx=20, pady=30, borderwidth=0)
  scanning_image_label.place(relx=0.5, rely=0.47, anchor=CENTER)
  scanning_text_label = Label(root, text="Scanning for", borderwidth=0, bg="White", fg="#0f62fe", font=("Arial ", 18))
  scanning_text_label.place(relx=0.5, rely=0.60, anchor = CENTER)
  name_label = Label(root, text="BLE_OTA_FWUP", borderwidth=0, bg="White", fg="#0f62fe", font=("Arial Bold", 20))
  name_label.place(relx=0.5, rely=0.67, anchor = CENTER )

  # (threading.Thread(target=fscan)).start()
  root.after(100, scan)
#Bleak functions
def scan():
  global k, dev_add, dev_name

  k = 0

  async def run():
      print("Scanning for BLE_OTA_FWUP...")
      return await BleakScanner.discover()

  def finish(exc, devices):
      global k, dev_add, dev_name
      if exc:
          print(exc)
          return
      if devices is None:
          return
      if len(devices) == 0:
          print("No devices found")
          return
      for d in devices:
          if d.name == "BLE_OTA_FWUP":
              print("BLE_OTA_FWUP found!")
              print("Address = ", d.address)
              dev_add = d.address
              dev_name = d.name
              print(dev_add)
              print("Initiating connection... ")
              root.after(400, found_im)
              break
          k += 1
          if k == len(devices):
              print("BLE_OTA_FWUP not found!!")
              print("Scanning Again")
              scan()

  _schedule_on_ble_loop(run(), finish)
def found_im():
  global connected_image_label
  global found_text_label
  global connecting_text_label

  previous_labels = [scanning_image_label,scanning_text_label,name_label]
  for label in previous_labels:
      label.destroy()

  connected_image_label = Label(root, image=conn_o_image, padx=20, pady=30, borderwidth=0, bg="White")
  connected_image_label.place(relx=0.5, rely=0.47, anchor=CENTER)
  found_text_label = Label(root, text="BLE_OTA_FWUP  found!", borderwidth=0, bg="White", fg="#0f62fe", font=("Arial Bold", 18))
  found_text_label.place(relx=0.5, rely=0.60, anchor=CENTER)
  connecting_text_label = Label(root, text="Connecting. Please wait...", borderwidth=0, bg="White", fg="#0f62fe", font=("Arial Bold", 20))
  connecting_text_label.place(relx=0.5, rely=0.67, anchor=CENTER)
  root.after(100,con)
def con():
  print("Connecting. Please wait..")

  async def connect_and_read():
      global client, connection_status, fwv, bd_address
      connection_status = False
      client = BleakClient(dev_add)
      try:
          await client.connect()
          connection_status = True
          print("Connected to BLE_OTA_FWUP")
      except Exception as e:
          connection_status = False
          print(e)
          return

      fwv = ""
      bd_address = ""

      await asyncio.sleep(BLE_POST_CONNECT_PAUSE_S)

      if OTA_SKIP_FW_VERSION_AND_BD_READS:
          bd_address = str(dev_add).upper().replace("-", ":")
          fwv = "(not read)"
          print(
              "Skipping GATT reads for FW version / BD address; using scan address:",
              bd_address,
          )
      else:
          try:
              m = await client.read_gatt_char(fw_version_uuid)
              byte_array = bytearray(m)
              converted_string = convert_bytearray_to_string(byte_array)
              first_converted_string = convert_firstbytearray_to_string(byte_array)
              print("converted_string:", hex(first_converted_string))
              hex_string = format(first_converted_string, "x")
              fv = f"{hex_string}.{converted_string}"
              fwv = fv
              print("Current FW version is:  ", fv)
          except Exception as e:
              print(e)
          try:
              raw_bd = await client.read_gatt_char(bd_address_uuid)
              if isinstance(raw_bd, (bytes, bytearray)):
                  null = raw_bd.find(b"\x00")
                  bd_s = raw_bd[: null if null != -1 else len(raw_bd)].decode(
                      "utf-8", errors="replace"
                  )
                  bd_address = bd_s.strip() or str(dev_add).upper().replace("-", ":")
              else:
                  bd_s = str(raw_bd)
                  if len(bd_s) >= 29:
                      bd_address = bd_s[12:29]
                  else:
                      bd_address = bd_s
              print("BD address of the module is:  ", bd_address)
          except Exception as e:
              print(e)
      if not client.is_connected:
          connection_status = False
          print("Device disconnected after GATT access; try connecting again.")

  async def connect_session():
      try:
          await connect_and_read()
      finally:
          if not connection_status:
              try:
                  if client is not None and getattr(client, "is_connected", False):
                      await client.disconnect()
              except Exception:
                  pass
              await asyncio.sleep(0.25)

  def finish(exc, _ignored):
      global connection_status
      if exc:
          connection_status = False
          print(exc)
      if connection_status:
          root.after(100, cond)
      else:
          print(f"Reconnecting in {BLE_RECONNECT_DELAY_MS // 1000}s...")
          root.after(BLE_RECONNECT_DELAY_MS, con)

  _schedule_on_ble_loop(connect_session(), finish)
def cond():
    global back_image_label,device_name_label,bd_address_label ,bt_logo_label
    global line_1a
    global line_1b
    global line_2a
    global line_2b
    global curfw_text_label
    global cur_fwv_value_label
    global fwup_button_label
    global fwv

    previous_labels = [connected_image_label, found_text_label, connecting_text_label]
    for label in previous_labels:
      label.destroy()
    back_image_label = Label(root, image=whiteback_image, padx=20, pady=30, borderwidth=0, bg="White")
    back_image_label.place(relx=0.47, rely=0.47, anchor=CENTER)
    device_name_label = Label(root, text=dev_name, borderwidth=0, bg="White", fg="#0f62fe", font=("Arial Bold", 20))
    device_name_label.place(x=260, y=140)
    bd_address_label = Label(root, text=bd_address or "(unavailable)", borderwidth=0, bg="White", fg="Grey", font=("Arial ", 15), height=1)
    bd_address_label .place(x=260, y=185)
    bt_logo_label = Label(root, image=conn_bt_image, borderwidth=0, bg="White")
    bt_logo_label.place(x=185, y=149)

    #line
    line_1a = Label(root, borderwidth=1, bg="Grey", padx=160, pady=0)
    line_1a.place(x=185, y=229)
    line_1b = Label(root, borderwidth=1, bg="White", padx=160, pady=0)
    line_1b.place(x=185, y=230)
    #end line

    curfw_text_label = Label(root, text="Current Firmware Version:", bg="White", fg="Black", font=("Arial Bold", 14))
    curfw_text_label.place(x=185, y=250)

    # line
    line_2a = Label(root, borderwidth=1, bg="Grey", padx=160, pady=0)
    line_2a.place(x=185, y=334)
    line_2b = Label(root, borderwidth=1, bg="White", padx=160, pady=0)
    line_2b.place(x=185, y=335)
    # end line

    cur_fwv_value_label = Label(root, text=fwv, bg="White", fg="#666666", font=("Arial ", 14))
    cur_fwv_value_label.place(x=185, y=285)
    # fwup button
    fwup_button_label = Button(root, image=fwup_button_image, bg="#f0f0f0", borderwidth=0, command=selectfw)
    fwup_button_label.place(x=255, y=485)
def selectfw():
    global fwselpath
    global prev_sel_path
    fwselpath = filedialog.askopenfilename( title='Select new firmware file')
    if len(fwselpath)==0:
        pass
    elif fwselpath[-4:] != ".rps" and fwselpath[-4:] != ".bin":
      print("Please select only '.rps/.bin' file")
      rpserror()
    else:
        prev_sel_path = fwselpath
        selected()
def selected():
    global newfw
    global start_fwup_button_label
    global sel_fw_text_label
    global sel_diff_fw_label
    global newfw_value_label
    global counter
    global selected_counter

    counter += 1

    if selected_counter>0:
        newfw_value_label.destroy()

    selected_counter += 1
    sel_fw_text_label = Label(root, text="Selected Firmware Version:", bg="White", fg="Black", font=("Arial Bold", 14))
    sel_fw_text_label.place(x=185, y=350)
    k = fwselpath.replace("/", "", fwselpath.count("/") - 1)
    p = k.index("/")
    newfw = k[p + 1:len(k) - 4]
    print("Selected fw version: ",newfw,"        ")
    newfw_value_label = Label(root, text=newfw, bg="White", fg="#666666", font=("Arial ", 14))
    newfw_value_label.place(x=185, y=385)
    fwup_button_label.destroy()
    fwup_button_label.destroy()

    if counter==1:
        sel_diff_fw_label = Button(root, text="Select different Firmware", command= selectfw)
        sel_diff_fw_label.place(relx=0.47, rely=0.8, anchor=CENTER)
        start_fwup_button_label = Button(root, image=start_fwup_button_image, bg="#f0f0f0", borderwidth=0, command=finalcheck)
        start_fwup_button_label.place(relx=0.47, rely=0.9, anchor=CENTER)
def rpserror():
  df = Toplevel()
  df.title(" Invalid file format")
  df.geometry("430x100")
  df.resizable(0,0)
  if (platform.system()) == "Windows":
      df.iconbitmap(sil_icon_path)
  title = Label(df, text="   Invalid format. Please select only '.rps' / '.bin' files  ",
                bd=2, height=2, width=47, fg="Blue", anchor="w",
                font=("Verdana 10 bold", 14))
  title.place(x=2, y=3)

  ok = Button(df, text="OK", bd=1, height=1, width=5,
              font=("Verdana 10 bold", 13), bg="white", fg="Blue", command=df.destroy)
  ok.place(x=130, y=60)
def finalcheck():
    global fp
    fp = Toplevel()
    fp.title("  Firmware Update over BLE")
    fp.geometry("500x180")
    fp.resizable(0,0)
    if (platform.system()) == "Windows":
      fp.iconbitmap(sil_icon_path)
    title = Label(fp, text=" Start firmware update?", bd=2, height=2, width=47,
                anchor="w", fg="#0f62fe", font=("Arial bold", 14))
    title.place(x=20, y=3)
    L90 = Label(fp, text="Current Firmware Version:",  fg="Black", font=("Arial Bold", 12))
    L90.place(x=30, y=50)
    L100 = Label(fp, text="Selected Firmware Version:",  fg="Black", font=("Arial Bold", 12))
    L100.place(x=30, y=80)

    #Final popup current fw label
    L223 = Label(fp, text=fwv, fg="#666666", font=("Arial ", 14))
    L223.place(x=255, y=47)

    L233 = Label(fp, text=newfw, fg="#666666", font=("Arial ", 14))
    L233.place(x=255, y=77)
    ok = Button(fp, image=start_fwup_button_image_2, borderwidth=0,command = final_start  ).place(x=130, y=120)
def final_start():
  global in_progress_text_label
  fp.destroy()
  previous_labels = [start_fwup_button_label, sel_diff_fw_label]
  for label in previous_labels:
      try:
        label.destroy()
      except:
          " "

  in_progress_text_label = Label(root, text="Firmware transfer in progress...", fg="#0f62fe", bg="white", font=("Arial Bold", 17))
  in_progress_text_label.place(relx=0.46, rely=0.72, anchor= CENTER)

  root.after(200,start_fw_transfer)
def convert(a):
  x=a//256
  y=a%256
  return bytearray([y,x])
def start_fw_transfer():
  global start_time
  global end_time
  start_time=time.time()

  async def run():
      global client
      global conn
      global total_chunks_encoded
      global chunk_number
      start_dt_tf = b'\x00'
      chunk_number=0
      print("Starting firmware Update")
      print("FW path =  ", prev_sel_path)
      fw_file = open(prev_sel_path, 'rb')  #opening the selected firmware file to read info about total number of chunks and last chunk size
      total = fw_file.read()
      total = len(total)  #total number of bytes in the firmware file
      chunks = (total // chunk_size) #total number of bytes in the firmware file
      last_chunk_size = (total) % chunk_size
      print("Size of the selected firmware file = ", total, "bytes = ", 1 + chunks + 1, "chunks")
      #print("Size of the 1st chunk       = 64 bytes")
      print("Size of each of the remaining chunks= ", chunk_size, "bytes")
      print("Size of the last chunk       =", last_chunk_size, "bytes ")
      fw_file.close()
      total_chunks_encoded = convert(chunks + 2)
      await client.write_gatt_char(prelim_info_uuid, bytearray(start_dt_tf), True)
      #await client.write_gatt_char(prelim_info_uuid, total_chunks_encoded, True)
      #await client.write_gatt_char(prelim_info_uuid, bytearray([last_chunk_size]), True)
      #fw_file = open(prev_sel_path, 'rb')
      #k = fw_file.read(64)
      #fw_file.close()
      fw_file = open(prev_sel_path, 'rb')
      #await client.write_gatt_char(fw_chunks_uuid, k, True)
      while 1:
          k = fw_file.read(chunk_size)
          if len(k) < chunk_size:
              try:
                  await client.write_gatt_char(fw_chunks_uuid, k, True)
                  break
              except:
                  break

          try:
              await client.write_gatt_char(fw_chunks_uuid, k, True)
          except:
              print("Module Disconnected. Please try again")
              exit()
          chunk_number+=1

  def finish(exc, _ignored):
      global update_status
      if exc:
          print(exc)
      update_status = 1
      print("Total time =", time.time() - start_time)
      root.after(500, done)

  _schedule_on_ble_loop(run(), finish)
def done():
  global safeup_prog_label
  previous_labels = [curfw_text_label, sel_fw_text_label, in_progress_text_label]
  for label in previous_labels:
      label.destroy()

  L19_1 = Label(root, text="Previous Firmware Version:", bg="White", fg="Black", font=("Arial Bold", 14))
  L19_1.place(x=185, y=250)
  L19_2 = Label(root, text="New Firmware Version:        ", bg="White", fg="Black", font=("Arial Bold", 14))
  L19_2.place(x=185, y=350)

  safeup_prog_label = Label(root, text="Safe upgrade in progress...", fg="#0f62fe", bg="white", font=("Arial Bold", 17))
  safeup_prog_label.place(relx=0.46, rely=0.72, anchor= CENTER)
  root.after(29000,last)
def last():
  safeup_prog_label.destroy()
  global success_label
  success_label = Label(root, text="Firmware Updated successfully!!", fg="Green", bg="white", font=("Arial Bold", 17))
  success_label.place(relx=0.47, rely=0.72, anchor= CENTER)

  Exit_button = Button(root, image=exit_button_image, command=exit, borderwidth=0)
  Exit_button.place(x=240, y=520)
  print("Firmware updated Successfully")

root = Tk()
root.geometry("730x615")
root.resizable(0,0)
root.title("  Firmware update over BLE")
root.call('wm', 'iconphoto', root._w, PhotoImage(file = icon_path))    # title icon
root.configure(background="#f0f0f0")
root.after(0, _poll_ble_gui_queue)



wb_image =           ImageTk.PhotoImage((Image.open(whiteback_path)).resize((720,600)))
bb_image =           ImageTk.PhotoImage((Image.open(blueback_path)).resize((720,600)))
im_image =           ImageTk.PhotoImage((Image.open(image_path)).resize((400,150)))
lotext_image =       ImageTk.PhotoImage((Image.open(silabs_logo_text_path)).resize((400,60)))
start_button_image = ImageTk.PhotoImage((Image.open(start_button_path)).resize((115,50)))
d1=                  ImageTk.PhotoImage((Image.open(d1_path)).resize((100,60)))
d2=                  ImageTk.PhotoImage((Image.open(d2_path)).resize((140,60)))
d3=                  ImageTk.PhotoImage((Image.open(d3_path)).resize((190,70)))
nearby_image = ImageTk.PhotoImage((Image.open(nearby_path)).resize((490,390)))
scan_o_image = ImageTk.PhotoImage((Image.open(scan_o_path)).resize((36,37)))
conn_o_image = ImageTk.PhotoImage((Image.open(conn_o_path)).resize((443,380)))
conn_wait_image = ImageTk.PhotoImage((Image.open(conn_wait_path)).resize((310,42)))
whiteback_image = ImageTk.PhotoImage((Image.open(whiteback_path)).resize((400,384)))
conn_bt_image = ImageTk.PhotoImage((Image.open(conn_bt_path)).resize((59,54)))
fwup_button_image = ImageTk.PhotoImage((Image.open(fwup_button_path)).resize((190,54)))
start_fwup_button_image = ImageTk.PhotoImage((Image.open(start_fwup_button_path)).resize((220,54)))
start_fwup_button_image_2 = ImageTk.PhotoImage((Image.open(start_fwup_button_path)).resize((180,44)))
exit_button_image = ImageTk.PhotoImage((Image.open(exit_button_path)).resize((180,44)))


L1=  Label(root, image=bb_image, padx=20, pady=30, borderwidth=0)
L1.place(x=0, y=0)

L2 = Label(root, image=im_image, padx=20, pady=30, borderwidth=0)
L2.place(x=163, y=203)

L3 = Label(root, image=lotext_image, padx=20, pady=30, borderwidth=0)
L3.place(x=163, y=363)

Start_button= Button(root,  image = start_button_image , command = lambda: [ anim1()], borderwidth=0 )
Start_button.place(x=310, y= 520)


root.mainloop()
