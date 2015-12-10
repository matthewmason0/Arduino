Public Class Form1
    Dim PadPos As Point
    Private Sub Pad_MouseUp(sender As Object, e As EventArgs) Handles Pad.MouseUp
        Dim WorkingArea As Rectangle
        Screen.GetWorkingArea(WorkingArea)
        Windows.Forms.Cursor.Clip = WorkingArea
    End Sub
    Private Sub Pad_MouseDown(sender As Object, e As EventArgs) Handles Pad.MouseDown
        Windows.Forms.Cursor.Clip = New Rectangle(PadPos, Pad.Size)
    End Sub
    Private Sub Pad_MouseMove(sender As Object, e As EventArgs) Handles Pad.MouseMove
        If MouseButtons = Windows.Forms.MouseButtons.Left Then
            Windows.Forms.Cursor.Clip = New Rectangle(PadPos, Pad.Size)
        End If
    End Sub
    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        PadPos = Pad.PointToScreen(New Point(0, 0))
    End Sub
    Private Sub Form1_Move(sender As Object, e As EventArgs) Handles MyBase.LocationChanged
        PadPos = Pad.PointToScreen(New Point(0, 0))
    End Sub

    Private Sub ExitButton_Click(sender As Object, e As EventArgs) Handles ExitButton.Click
        Me.Close()
    End Sub
End Class
