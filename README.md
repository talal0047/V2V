
## READ THIS BEFORE INSTALLING

This plugin uses UPnP port forwarding to host servers. Only use this plugin if your router and ISP support it.  

---

## Prerequisites:

This plugin requires the advanced sessions plugin which you can download from any of these sources:

Advanced Session Binaries: https://vreue4.com/advanced-sessions-binaries

Github Repo: https://github.com/mordentral/AdvancedSessionsPlugin

---

# 🧩 V2V Plugin Installation Guide (Unreal Engine 5.5)

## 📥 Step 1: Download or Clone the Plugin

You can install the plugin by either cloning it or downloading it as a ZIP.

### ✅ Option A: Clone via Git

```bash
git clone https://github.com/talal0047/V2V.git
```

Then move the `V2V` folder to your Unreal project’s `Plugins` directory:

```bash
mv V2V /YourUEProject/Plugins/
```

### ✅ Option B: Download ZIP

1. Visit the plugin's [GitHub page](https://github.com/talal0047/V2V).
2. Click the green **Code** button → **Download ZIP**.
3. Extract the ZIP.
4. Move the extracted `V2V` folder into your project’s `Plugins` directory:

```
/YourUEProject/Plugins/V2V
```

---

## 🧱 Step 2: Generate Project Files

Since V2V contains C++ code, you’ll need to regenerate your `.sln` file:

1. **Right-click** on your `.uproject` file.
2. Select **Generate Visual Studio project files**.
3. This creates or updates your `.sln` with the V2V plugin code included.

note: if your project is blueprint only, it'll not generate the solution. To work around that, you must open your project and add a new C++ file by going into tools-> new c++ class. You may select it as empty.

---

## 🛠 Step 3: Build the Plugin (Visual Studio)

1. Open the generated `.sln` file in **Visual Studio 2022** or newer.
2. Make sure the build configuration is set to:

   * **Development Editor**
   * **Win64**
3. Right-click on your **.uproject** target in the **Solution Explorer**.
4. Click **Build**.

After a successful build, you’ll find the compiled plugin binaries in:

```
/YourUEProject/Binaries/
/YourUEProject/Intermediate/
/YourUEProject/Plugins/V2V/Binaries/
```

---

## 🧩 Step 4: Enable the Plugin in UE5.5

1. Open the Unreal Engine Editor.
2. Go to **Edit → Plugins**.
3. Search for **V2V**.
4. Check the box to enable it.
5. Restart Unreal when prompted.

---

## ✅ Step 5: Verify Installation

* After restarting, go to **Edit → Plugins** and confirm **V2V** is marked as **Enabled**.
* In your content drawer, under plugins you should see the V2V and V2V C++ Classes folder.

> 📷 Plugin Verification:
> ![V2VPlugin](images/V2VPluginContentDrawer.png)

---

## 🔄 Updating the Plugin

If installed via Git:

```bash
cd /YourUEProject/Plugins/V2V
git pull
```

Then regenerate project files and **rebuild the solution in Visual Studio**.

If installed via ZIP, replace the plugin folder and repeat the build steps.

---

# V2V Plugin Example Usage (You can skip this section if you wan't to do your own implementation)

This setup will guide you how to use the example included in the plugin. 

## Step 1: Open the Example map

1. Open Content Drawer
2. Go to All -> Plugins -> V2V Content -> Level
3. Open ExampleMap (it's just an empty level with the exception of one blueprint so don't panic if you see a black screen in the editor 🙂)

---

## Step 2: Setting your Server Map  

1. Open V2V Content -> Blueprints -> BP_Exanple_ServerConnection
2. Find the Host Server Node, it's in the sequence of OnHostButtonCLicked Event.
3. Change the Map Name to your server map, i.e. the map you want to open when the server is started. 
4. By default the server runs aat 7777 port, if you want to change it you need to change it in **Host Server** node as well as the **Try Activating Upnp** node.

> 📷 Host Server:
> ![Host Server](images/HostServerNode.png)
   
---

## Step 3: Hooking events

Now you need to hook some events with your gamemode and game instance

### Game Mode: 
1. Open your target map Game Mode 
2. Add **BPC_V2VPlayerSessionController** Component.
3. In the event graph, hook **OnPostLogin** and **OnLogout** to the ones in **BPC_V2VPlayerSessionController** as shown below:

> 📷 Game Mode Setup:
> ![Game Mode](images/GameModeSetup.png)


### Game Instance:
1. Open your target map Game Instance 
2. In the event graph, add the **Event Shutdown** note, then get actor of class (**BP_Example_ServerConnection**) and from it, call the **OnGameShutdown** event. See the image below:

> 📷 Game Instance Setup:
> ![Game Instance](images/GameInstanceSetup.png)

---

## Step 4: Setting Up Kick Player UI

1. In the blurprints folder there's an actor **BP_Example_KickPlayerMenuController** that handles the kick player Ui, open it.
2. If you are using using VR, set the isVR bool in variables as true, otherwise set it to false. 
3. Now open your target map (the server map which you used in step 2) and drag & drop **BP_Example_KickPlayerMenuController** into the map.
4. If you're using VR, you'll need to drag & drop **BP_Example_VR_KickPlayerMenu** as well as it uses Ui in world space.

> 📷 Kick Player UI Setup:
> ![Kick Player UI](images/KickPlayerUI.png)

---

## Step 5: Play 

Now you're ready to play the example.
1. Load the ExampleMap and press play. 
2. You should see the following UI 

> 📷 Server Connection UI Host:
> ![Server Connection UI 1](images/ServerConnectionUI1.png)

3. Now if you want to use the UPnP, check the Auto Port Forwarding and press Host.

> 📷 Server Connection UI Start Server:
> ![Server Connection UI 1](images/ServerConnectionUI2.png)

4. You will see your server IP and next to it the copy button which you can share with you friend.
5. Click Start Server button to start the server 

> 📷 Server Connection UI Join:
> ![Server Connection UI 1](images/ServerConnectionUI3.png)

6. To join simply paste the IP in the first textbox and press join. 
7. Now you should be in the server together. 
8. To bring the kick player menu you can press Tab if you're not in VR. If you're in VR it should appear where you placed it. 
9. Now you're done with the Example Usage. 

---

# Implementation and usage of V2V Component 

This section focuses on the usage and explanation of V2V component which is based on C++ but the functions are callable via blueprint.

---

## Adding the V2V Component 

1. Open your desired actor and add the **V2V** component to it.

> 📷 V2V Component:
> ![V2V Component](images/V2VComponent.png)

---

## Usage of UPnP Functions

1. In the event graph of your actor with the V2V component attached, drag and drop the V2V component use the **Try Activating Upnp** and **End Upnp Activation** End nodes from it.
2. The **Try Activating Upnp** takes 2 inputs i.e. the V2V Component reference and the port. You can set the port as 7777. It also gives out a boolean output telling if the Upnp activation was successful or not. 
3. The **End Upnp Activation** takes just 1 input i.e. the V2V Component reference and you should call it from something like **Event Shutdown** from game instance. 

> 📷 Try Activating Upnp:
> ![Try Activating Upnp](images/TryActivatingUpnp.png)

> 📷 End Upnp Activation:
> ![End Upnp Activation](images/EndUpnpActivation.png)

---

## Usage of Host Server

1. **Host Server** is a static function so you can call it from anywhere. 
2. The **Host Server** node takes 5 inputs i.e. Game Instance Reference, Map Name String, Port int, bool Enable Listen Server and Bool Absolute.  
3. You should set the Map name as the map you want to load, port as the server port (usually set as 7777) and check Enable Listen server while keeping the desired value of absolute. 

> 📷 Host Server:
> ![Host Server](images/HostServer.png)

---

## Usage of Join Server

1. Like Host Server, **Join Server** is also a static function and can be called from anywhere. 
2. The **Join Server** node takes in 2 inputs. i.e. Game Instance Reference and IP. 
3. The IP should be your external IP which you can get via another function explained next. 

> 📷 Join Server:
> ![Join Server](images/JoinServer.png)

---

## Usage of Get External IP

1. This function can be called from the V2V Component. 
2. When it is called for the first time it doesn't give the return value right away since it makes an **https GET** call which takes some time to respond. Generally 1-1.5 seconds. To work around that, add a delay in your blueprint then get the External IP directly. 
3. When it is called anytime after that it returns the previously received external IP. 

> 📷 Get External IP:
> ![Get External IP](images/GetExternalIP.png)

--- 

# Implementation and usage of BPC_V2VPlayerSessionController

The **BPC_V2VPlayerSessionController** is a blueprint component that should be attached to your Game Mode. It maintains an array of players and offers an Event to Kick Player from the server. 

---

## Implementation

1. Open your target map Game Mode 
2. Add **BPC_V2VPlayerSessionController** Component.
3. In the event graph, hook **OnPostLogin** and **OnLogout** to the ones in **BPC_V2VPlayerSessionController** as shown below:

> 📷 Game Mode Setup:
> ![Game Mode](images/GameModeSetup.png)

---

## Usage of OnPostLogin

**OnPostLogin** takes 1 input i.e. the player controller of the joining player. It adds the **new player** to the **Players Array**.

> 📷 On Post Login:
> ![On Post Login](images/OnPostLogin.png)

---

## Usage of OnLogout

**OnLogout** takes 1 input i.e. the player controller of the logged out player. It removes the **Logged Out Player** from the **Players Array**.

> 📷 On Logout:
> ![On Logout ](images/OnLogout.png)

---

## Usage of KickPlayerByIndex

1. **KickPlayerByIndex** takes 2 inputs i.e. the **Player Index** and **Reason**.
2. The **Player Index** is the index of the player from the **Players Array** to be kicked.
3. The **Reason** is the reason why the player is kicked from the server. 
4. The **Kick Player** function is from the **Advanced Sessions** Library that takes 2 inputs i.e. the **PlayerController** of the player to kick and **Kick Reason** as text, and 1 bool output which returns whether the kick was successful or not. 

> 📷 Kick Player By Index:
> ![Kick Player By Index ](images/KickPlayerByIndex.png)