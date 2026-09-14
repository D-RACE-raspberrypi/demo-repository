echo "  Starting installation :"
# Docker installation script
echo "Starting Docker installation script..."
sudo rm -f /etc/apt/sources.list.d/docker.list
echo "Updating package lists and installing prerequisites..."
sudo apt update
sudo apt install -y ca-certificates curl gnupg
echo "Setting up Docker keyrings and repository..."
sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/debian/gpg | sudo tee /etc/apt/keyrings/docker.asc > /dev/null
sudo chmod a+r /etc/apt/keyrings/docker.asc
echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/debian $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
echo "Removing old Docker buildx plugin if it exists..."
sudo apt remove -y docker-buildx
sudo apt update
echo "Installing Docker packages..."
sudo apt install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
sudo usermod -aG docker $USER
echo "Adding current user to the Docker group..."
newgrp docker
echo "Docker installation complete."

# Git clone
echo "Cloning demo repository..."
git clone https://github.com/D-RACE-raspberrypi/demo-repository.git
echo "Demo repository cloned."

# Service
echo "Setting up rpi-serv systemd service..."
sudo tee /etc/systemd/system/rpi-serv.service > /dev/null <<EOL
[Unit]
Description=Démarrage automatique du projet Docker Compose
Requires=docker.service
After=docker.service network-online.target
Wants=network-online.target

[Service]
Type=oneshot
RemainAfterExit=yes
WorkingDirectory=/home/drace/demo-repository/rpi-serv
ExecStart=/usr/bin/docker compose up -d
ExecStop=/usr/bin/docker compose down

[Install]
WantedBy=multi-user.target
EOL
echo "rpi-serv systemd service setup complete."
sudo systemctl daemon-reload
sudo systemctl enable rpi-serv.service
sudo systemctl start rpi-serv.service
echo "rpi-serv service started."

# End of installation script
echo "  Installation script complete."