# Running ePDFView in WSL (Windows Subsystem for Linux)

## The Issue

ePDFView needs a display server to show its GUI. In WSL, you need to ensure the DISPLAY variable is set.

## Solution

### For Windows 11 with WSLg (Built-in)

WSLg is built into Windows 11 and provides automatic GUI support. You just need to set the DISPLAY variable:

```bash
# In WSL Arch
export DISPLAY=:0
epdfview
```

### Create a Wrapper Script

Create a helper script to always set DISPLAY:

```bash
# Create the script
sudo tee /usr/local/bin/epdfview-wsl > /dev/null << 'EOF'
#!/bin/bash
export DISPLAY=:0
export WAYLAND_DISPLAY=wayland-0
exec /usr/local/bin/epdfview "$@"
EOF

# Make it executable
sudo chmod +x /usr/local/bin/epdfview-wsl

# Now you can just run:
epdfview-wsl
```

### Add to Your Shell Profile

To automatically set DISPLAY for all GUI apps:

```bash
# Add to ~/.bashrc or ~/.zshrc
echo 'export DISPLAY=:0' >> ~/.bashrc
source ~/.bashrc
```

## Troubleshooting

### "bogus screen size" Error

This is a WSLg issue with display detection. ePDFView now handles this automatically by using sensible defaults (800x600) when invalid dimensions are detected.

### Window Doesn't Appear

1. **Check if WSLg is running:**
   ```bash
   ls -la /mnt/wslg/
   ```
   Should show X11-unix, runtime-dir, etc.

2. **Check DISPLAY is set:**
   ```bash
   echo $DISPLAY
   ```
   Should show `:0` or similar

3. **Test with a simple GUI app:**
   ```bash
   # Install xeyes for testing
   sudo pacman -S xorg-xeyes
   export DISPLAY=:0
   xeyes
   ```
   If xeyes works, epdfview should too.

4. **Check if process is running:**
   ```bash
   export DISPLAY=:0
   epdfview &
   ps aux | grep epdfview
   ```

### D-Bus Warning

The warning "Unable to acquire session bus" is normal in WSL and doesn't affect functionality. It's just informational.

### libEGL Warning

The "DRI3 error" warning is also normal in WSL without full GPU acceleration. It doesn't prevent the app from working.

## For Windows 10 (Without WSLg)

If you're on Windows 10 without WSLg, you need an X server:

1. **Install VcXsrv or Xming** (Windows X server)
   - Download VcXsrv: https://sourceforge.net/projects/vcxsrv/
   - Or Xming: https://sourceforge.net/projects/xming/

2. **Start the X server** with:
   - Display number: 0
   - Disable access control

3. **Set DISPLAY in WSL:**
   ```bash
   export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
   epdfview
   ```

## Testing

To test if everything is working:

```bash
# Set display
export DISPLAY=:0

# Run epdfview (should stay running)
epdfview &

# Check if process exists
sleep 1
ps aux | grep epdfview

# If you see the process, the window should be visible!
```

## Native Arch Linux (Not WSL)

If you're running on real Arch Linux (not WSL):

```bash
# Just run it - no special setup needed!
epdfview
```

The DISPLAY variable should already be set by your desktop environment.

## Summary

**WSL (Windows 11):**
```bash
export DISPLAY=:0
epdfview
```

**WSL (Windows 10):**
- Install VcXsrv
- `export DISPLAY=<windows-ip>:0`
- `epdfview`

**Native Linux:**
- Just run `epdfview`

---

**If the window appears, ePDFView is working correctly!** 🎉

The warnings about D-Bus and libEGL are normal and can be ignored.
