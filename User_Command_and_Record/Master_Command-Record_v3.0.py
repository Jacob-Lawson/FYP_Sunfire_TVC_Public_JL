import wx  # Importing the wxPython library for creating GUI applications
import subprocess  # For running subprocesses
import threading  # For running threads in parallel
import logging  # For logging errors
import os  # For interacting with the operating system

# Get the directory of the parent GUI script
script_directory = os.path.dirname(os.path.realpath(__file__))

# Construct the path for the log file in the same directory as the script
log_file_path = os.path.join(script_directory, 'gui.log')

# Configure logging to write errors to the log file
logging.basicConfig(filename=log_file_path, level=logging.ERROR)

# Define the main frame of the GUI
class MyFrame(wx.Frame):
    def __init__(self, parent, title):
        super(MyFrame, self).__init__(parent, title=title, size=(500, 550))

        panel = wx.Panel(self)

        vbox = wx.BoxSizer(wx.VERTICAL)

        # Title for user entry fields
        user_settings_title = wx.StaticText(panel, label="User TVC Test Settings", style=wx.ALIGN_CENTER)
        user_settings_title.SetFont(wx.Font(wx.FontInfo(12).Bold()))
        vbox.Add(user_settings_title, flag=wx.EXPAND | wx.TOP | wx.CENTER, border=10)

        vbox.Add((-1, 10))  # Add spacing between sections

        grid = wx.GridBagSizer(hgap=10, vgap=10)

        # IP Address label and text entry field
        ip_label = wx.StaticText(panel, label="IP Address:")
        ip_label.SetFont(wx.Font(wx.FontInfo(10).Bold()))
        grid.Add(ip_label, pos=(0, 0), flag=wx.ALIGN_LEFT | wx.ALIGN_CENTER_VERTICAL)

        self.ip_entry = wx.TextCtrl(panel)
        grid.Add(self.ip_entry, pos=(0, 1), span=(1, 3), flag=wx.EXPAND | wx.ALL, border=5)

        # API Script label, text entry field, and browse button
        api_script_label = wx.StaticText(panel, label="Python API Script:")
        api_script_label.SetFont(wx.Font(wx.FontInfo(10).Bold()))
        grid.Add(api_script_label, pos=(1, 0), flag=wx.ALIGN_LEFT | wx.ALIGN_CENTER_VERTICAL)

        # Get the path of the GUI script
        gui_script_path = os.path.abspath(__file__)
        default_api_script_path = os.path.join(os.path.dirname(gui_script_path), "Slave_Record_v2.0.py")

        self.api_entry = wx.TextCtrl(panel, value=default_api_script_path)  # Set default value
        grid.Add(self.api_entry, pos=(1, 1), span=(1, 2), flag=wx.EXPAND | wx.ALL, border=5)

        self.api_browse_button = wx.Button(panel, label="Browse")
        grid.Add(self.api_browse_button, pos=(1, 3), flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL, border=5)


        # Command Profile Script label, text entry field, and browse button
        command_profile_label = wx.StaticText(panel, label="Command Profile Script:")
        command_profile_label.SetFont(wx.Font(wx.FontInfo(10).Bold()))
        grid.Add(command_profile_label, pos=(2, 0), flag=wx.ALIGN_LEFT | wx.ALIGN_CENTER_VERTICAL)

        # Get the path of the GUI script
        gui_script_path = os.path.abspath(__file__)
        default_command_profile_script_path = os.path.join(os.path.dirname(gui_script_path), "Slave_Command_v1.0.py")

        self.command_profile_entry = wx.TextCtrl(panel, value=default_command_profile_script_path)  # Set default value
        grid.Add(self.command_profile_entry, pos=(2, 1), span=(1, 2), flag=wx.EXPAND | wx.ALL, border=5)

        self.command_profile_browse_button = wx.Button(panel, label="Browse")
        grid.Add(self.command_profile_browse_button, pos=(2, 3), flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL, border=5)


        # CSV File label, text entry field, and browse button
        csv_label = wx.StaticText(panel, label="CSV File:")
        csv_label.SetFont(wx.Font(wx.FontInfo(10).Bold()))
        grid.Add(csv_label, pos=(3, 0), flag=wx.ALIGN_LEFT | wx.ALIGN_CENTER_VERTICAL)

        self.csv_entry = wx.TextCtrl(panel)
        grid.Add(self.csv_entry, pos=(3, 1), span=(1, 2), flag=wx.EXPAND | wx.ALL, border=5)

        self.csv_browse_button = wx.Button(panel, label="Browse")
        grid.Add(self.csv_browse_button, pos=(3, 3), flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL, border=5)

        # CSV Save Location label, text entry field, and browse button
        csv_save_location_label = wx.StaticText(panel, label="CSV Save Location:")
        csv_save_location_label.SetFont(wx.Font(wx.FontInfo(10).Bold()))
        grid.Add(csv_save_location_label, pos=(4, 0), flag=wx.ALIGN_LEFT | wx.ALIGN_CENTER_VERTICAL)

        self.csv_save_location_entry = wx.TextCtrl(panel)
        grid.Add(self.csv_save_location_entry, pos=(4, 1), span=(1, 2), flag=wx.EXPAND | wx.ALL, border=5)

        self.csv_save_location_browse_button = wx.Button(panel, label="Browse")
        grid.Add(self.csv_save_location_browse_button, pos=(4, 3), flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL, border=5)

        vbox.Add(grid, proportion=1, flag=wx.EXPAND | wx.ALL, border=10)

        # Title for user controls
        user_controls_title = wx.StaticText(panel, label="User TVC Test Controls", style=wx.ALIGN_CENTER)
        user_controls_title.SetFont(wx.Font(wx.FontInfo(12).Bold()))
        vbox.Add(user_controls_title, flag=wx.EXPAND | wx.TOP | wx.CENTER, border=10)

        vbox.Add((-1, 10))  # Add spacing between sections

        hbox_buttons1 = wx.BoxSizer(wx.HORIZONTAL)

        # Buttons for running scripts
        self.api_button = wx.Button(panel, label="Run Record API Script")
        hbox_buttons1.Add(self.api_button, flag=wx.LEFT | wx.RIGHT, border=5)

        self.command_profile_button = wx.Button(panel, label="Run Command Profile Script")
        hbox_buttons1.Add(self.command_profile_button, flag=wx.LEFT | wx.RIGHT, border=5)

        self.run_button = wx.Button(panel, label="Run All")
        hbox_buttons1.Add(self.run_button, flag=wx.LEFT | wx.RIGHT, border=5)

        vbox.Add(hbox_buttons1, flag=wx.EXPAND | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=10)

        hbox_buttons2 = wx.BoxSizer(wx.HORIZONTAL)

        # Buttons for stopping scripts
        self.api_stop_button = wx.Button(panel, label="Stop API Script")
        hbox_buttons2.Add(self.api_stop_button, flag=wx.LEFT | wx.RIGHT, border=5)

        self.command_profile_stop_button = wx.Button(panel, label="Stop Command Profile Script")
        hbox_buttons2.Add(self.command_profile_stop_button, flag=wx.LEFT | wx.RIGHT, border=5)

        vbox.Add(hbox_buttons2, flag=wx.EXPAND | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=10)

        # Title for status LEDs
        status_leds_title = wx.StaticText(panel, label="TVC GUI Status LEDs", style=wx.ALIGN_CENTER)
        status_leds_title.SetFont(wx.Font(wx.FontInfo(12).Bold()))
        vbox.Add(status_leds_title, flag=wx.EXPAND | wx.TOP | wx.CENTER, border=10)

        vbox.Add((-1, 10))  # Add spacing between sections

        hbox_status = wx.BoxSizer(wx.HORIZONTAL)

        # LED indicators for script status
        api_led_label = wx.StaticText(panel, label="API Script Status:")
        api_led_label.SetFont(wx.Font(wx.FontInfo(10).Bold()))
        hbox_status.Add(api_led_label, flag=wx.ALIGN_LEFT | wx.LEFT | wx.TOP, border=5)

        self.api_led = wx.StaticText(panel, label="Yet to run")
        self.api_led.SetBackgroundColour(wx.RED)
        hbox_status.Add(self.api_led, flag=wx.ALIGN_LEFT | wx.LEFT | wx.TOP | wx.BOTTOM, border=5)

        command_profile_led_label = wx.StaticText(panel, label="Command Profile Script Status:")
        command_profile_led_label.SetFont(wx.Font(wx.FontInfo(10).Bold()))
        hbox_status.Add(command_profile_led_label, flag=wx.ALIGN_LEFT | wx.LEFT | wx.TOP, border=5)

        self.command_profile_led = wx.StaticText(panel, label="Yet to run")
        self.command_profile_led.SetBackgroundColour(wx.RED)
        hbox_status.Add(self.command_profile_led, flag=wx.ALIGN_LEFT | wx.LEFT | wx.TOP | wx.BOTTOM, border=5)

        vbox.Add(hbox_status, flag=wx.ALIGN_CENTER | wx.TOP | wx.BOTTOM, border=10)

        panel.SetSizer(vbox)

        # Bind events
        self.api_browse_button.Bind(wx.EVT_BUTTON, self.on_api_browse)
        self.command_profile_browse_button.Bind(wx.EVT_BUTTON, self.on_command_profile_browse)
        self.csv_browse_button.Bind(wx.EVT_BUTTON, self.on_csv_browse)
        self.csv_save_location_browse_button.Bind(wx.EVT_BUTTON, self.on_csv_save_location_browse)
        self.api_button.Bind(wx.EVT_BUTTON, self.on_run_api)
        self.command_profile_button.Bind(wx.EVT_BUTTON, self.on_run_command_profile)
        self.run_button.Bind(wx.EVT_BUTTON, self.on_run_all)
        self.api_stop_button.Bind(wx.EVT_BUTTON, self.on_stop_api_script)
        self.command_profile_stop_button.Bind(wx.EVT_BUTTON, self.on_stop_command_profile_script)

        # Tooltip for API LED
        self.api_led.SetToolTip(wx.ToolTip("API Script Status:\nGreen - Running\nYellow - Ready\nRed - Yet to run"))

        # Tooltip for Command Profile LED
        self.command_profile_led.SetToolTip(wx.ToolTip("Command Profile Status:\nGreen - Running\nYellow - Ready\nRed - Yet to run"))

    # Event handler for browsing API script file
    def on_api_browse(self, event):
        dlg = wx.FileDialog(
            self, message="Select Python API Script",
            defaultDir=os.getcwd(),
            wildcard="Python files (*.py)|*.py|All files (*.*)|*.*",
            style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST
        )
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.api_entry.SetValue(path)
        dlg.Destroy()

    # Event handler for browsing command profile script file
    def on_command_profile_browse(self, event):
        dlg = wx.FileDialog(
            self, message="Select Command Profile Script",
            defaultDir=os.getcwd(),
            wildcard="Python files (*.py)|*.py|All files (*.*)|*.*",
            style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST
        )
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.command_profile_entry.SetValue(path)
        dlg.Destroy()

    # Event handler for browsing CSV file
    def on_csv_browse(self, event):
        dlg = wx.FileDialog(
            self, message="Select CSV File",
            defaultDir=os.getcwd(),
            wildcard="CSV files (*.csv)|*.csv|All files (*.*)|*.*",
            style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST
        )
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.csv_entry.SetValue(path)
        dlg.Destroy()

    # Event handler for browsing CSV save location
    def on_csv_save_location_browse(self, event):
        dlg = wx.DirDialog(self, message="Select CSV Save Location", defaultPath=os.getcwd(), style=wx.DD_DEFAULT_STYLE)
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.csv_save_location_entry.SetValue(path)
        dlg.Destroy()

    # Function to run a script in a thread
    def run_script(self, script_name, *args):
        try:
            subprocess.call(['python', script_name, *args])
        except Exception as e:
            logging.error(f"Error running script {script_name}: {e}")

    # Function to run the API script
    def run_api_script(self, ip_address, api_script, csv_save_location):
        try:
            threading.Thread(target=self.run_script, args=(api_script, ip_address, csv_save_location), daemon=True).start()
            self.api_led.SetLabel("Running")
            self.api_led.SetBackgroundColour(wx.GREEN)
        except Exception as e:
            logging.error(f"Error running API script: {e}")

    # Function to run the command profile script
    def run_command_profile_script(self, ip_address, command_profile_script, csv_file):
        try:
            threading.Thread(target=self.run_script, args=(command_profile_script, ip_address, csv_file), daemon=True).start()
            self.command_profile_led.SetLabel("Running")
            self.command_profile_led.SetBackgroundColour(wx.GREEN)
        except Exception as e:
            logging.error(f"Error running command profile script: {e}")

    # Function to stop the API script
    def stop_api_script(self):
        try:
            self.api_led.SetLabel("Stopped")
            self.api_led.SetBackgroundColour(wx.RED)
        except Exception as e:
            logging.error(f"Error stopping API script: {e}")

    # Function to stop the command profile script
    def stop_command_profile_script(self):
        try:
            self.command_profile_led.SetLabel("Stopped")
            self.command_profile_led.SetBackgroundColour(wx.RED)
        except Exception as e:
            logging.error(f"Error stopping command profile script: {e}")

    # Event handler for running the API script
    def on_run_api(self, event):
        ip_address = self.ip_entry.GetValue()
        api_script = self.api_entry.GetValue()
        csv_save_location = self.csv_save_location_entry.GetValue()

        if not ip_address or not api_script or not csv_save_location:
            wx.MessageBox("Please fill in all fields.", "Error", wx.OK | wx.ICON_ERROR)
            return

        self.run_api_script(ip_address, api_script, csv_save_location)

    # Event handler for running the command profile script
    def on_run_command_profile(self, event):
        ip_address = self.ip_entry.GetValue()
        command_profile_script = self.command_profile_entry.GetValue()
        csv_file = self.csv_entry.GetValue()

        if not ip_address or not command_profile_script or not csv_file:
            wx.MessageBox("Please fill in all fields.", "Error", wx.OK | wx.ICON_ERROR)
            return

        self.run_command_profile_script(ip_address, command_profile_script, csv_file)

    # Event handler for running all scripts
    def on_run_all(self, event):
        self.on_run_api(event)
        self.on_run_command_profile(event)

    # Event handler for stopping the API script
    def on_stop_api_script(self, event):
        self.stop_api_script()

    # Event handler for stopping the command profile script
    def on_stop_command_profile_script(self, event):
        self.stop_command_profile_script()


if __name__ == "__main__":
    app = wx.App()
    frame = MyFrame(None, "Parent Script GUI: Command_Record")
    frame.Show()
    app.MainLoop()
