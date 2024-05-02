import wx
import subprocess
import os
from datetime import datetime
import pandas as pd
from pandas import DataFrame
from pandas_profiling import ProfileReport
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import plotly.io as pio
from docx import Document
from docx.shared import Inches
from docx2pdf import convert

class MyFrame(wx.Frame):
    def __init__(self, parent, title):
        super(MyFrame, self).__init__(parent, title=title, size=(400, 300))

        self.InitUI()
        self.Centre()
        self.Show()

    def InitUI(self):
        panel = wx.Panel(self)

        vbox = wx.BoxSizer(wx.VERTICAL)

        # Add a label for the script
        label_script = wx.StaticText(panel, label="Choose a Python script to run:")
        vbox.Add(label_script, flag=wx.TOP | wx.LEFT | wx.BOTTOM, border=10)

        # Get the path of the GUI script
        gui_script_path = os.path.abspath(__file__)
        default_script_path = os.path.join(os.path.dirname(gui_script_path), "Slave_Analysis_V1.0.py")

        # Add a file picker for the script
        self.file_picker_script = wx.FilePickerCtrl(panel, message="Select a script", wildcard="Python files (*.py)|*.py")
        # Set a default file
        self.file_picker_script.SetPath(default_script_path)
        vbox.Add(self.file_picker_script, flag=wx.EXPAND | wx.LEFT | wx.RIGHT, border=10)

        # Add a label for the CSV file
        label_csv = wx.StaticText(panel, label="Choose a CSV file for analysis:")
        vbox.Add(label_csv, flag=wx.TOP | wx.LEFT | wx.BOTTOM, border=10)

        # Add a file picker for the CSV file
        self.file_picker_csv = wx.FilePickerCtrl(panel, message="Select a CSV file", wildcard="CSV files (*.csv)|*.csv")
        vbox.Add(self.file_picker_csv, flag=wx.EXPAND | wx.LEFT | wx.RIGHT, border=10)

        # Add a label for the directory
        label_dir = wx.StaticText(panel, label="Choose a directory to save the analysis results:")
        vbox.Add(label_dir, flag=wx.TOP | wx.LEFT | wx.BOTTOM, border=10)

        # Add a directory picker for the directory
        self.dir_picker = wx.DirPickerCtrl(panel, message="Select a directory")
        vbox.Add(self.dir_picker, flag=wx.EXPAND | wx.LEFT | wx.RIGHT, border=10)

        # Add a 'Run' button
        btn_run = wx.Button(panel, label='Run')
        btn_run.Bind(wx.EVT_BUTTON, self.OnRun)
        vbox.Add(btn_run, flag=wx.EXPAND | wx.ALL, border=10)

        panel.SetSizer(vbox)

    def OnRun(self, event):
        script_path = self.file_picker_script.GetPath()
        csv_path = self.file_picker_csv.GetPath()
        dir_path = self.dir_picker.GetPath()
        if script_path and csv_path and dir_path:
            try:
                # Run the selected script
                process = subprocess.Popen(['python', script_path, csv_path, dir_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                stdout, stderr = process.communicate()
                if process.returncode == 0:
                    wx.MessageBox("Script executed successfully!", "Success", wx.OK | wx.ICON_INFORMATION)
                else:
                    wx.MessageBox(f"An error occurred:\n{stderr.decode()}", "Error", wx.OK | wx.ICON_ERROR)
            except Exception as e:
                wx.MessageBox(str(e), "Error", wx.OK | wx.ICON_ERROR)


def main():
    app = wx.App()
    MyFrame(None, title='Parent Script GUI: Data_Analysis')
    app.MainLoop()


if __name__ == '__main__':
    main()
