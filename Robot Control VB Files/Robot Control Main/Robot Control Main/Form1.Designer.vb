<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.Pad = New System.Windows.Forms.PictureBox()
        Me.ExitButton = New System.Windows.Forms.Button()
        CType(Me.Pad, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'Pad
        '
        Me.Pad.BackgroundImage = Global.Robot_Control_Main.My.Resources.Resources.Pad
        Me.Pad.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None
        Me.Pad.Cursor = System.Windows.Forms.Cursors.Cross
        Me.Pad.Location = New System.Drawing.Point(29, 29)
        Me.Pad.Margin = New System.Windows.Forms.Padding(20)
        Me.Pad.Name = "Pad"
        Me.Pad.Size = New System.Drawing.Size(201, 201)
        Me.Pad.TabIndex = 1
        Me.Pad.TabStop = False
        '
        'ExitButton
        '
        Me.ExitButton.Location = New System.Drawing.Point(237, 3)
        Me.ExitButton.Name = "ExitButton"
        Me.ExitButton.Size = New System.Drawing.Size(18, 21)
        Me.ExitButton.TabIndex = 2
        Me.ExitButton.Text = "X"
        Me.ExitButton.UseVisualStyleBackColor = True
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(259, 259)
        Me.Controls.Add(Me.ExitButton)
        Me.Controls.Add(Me.Pad)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None
        Me.MinimumSize = New System.Drawing.Size(259, 259)
        Me.Name = "Form1"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.Manual
        Me.Text = "Form1"
        CType(Me.Pad, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Pad As System.Windows.Forms.PictureBox
    Friend WithEvents ExitButton As System.Windows.Forms.Button

End Class
