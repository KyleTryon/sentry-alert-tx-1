# MQTT Setup Guide

Based on the proven approach from [setup-mosquitto-with-docker](https://github.com/sukesh-ak/setup-mosquitto-with-docker).

## 🚀 **Quick Setup Process**

### **1. Start the MQTT Broker**
```bash
cd Beeper-Service
docker-compose up -d
```

### **2. Find the MQTT Container ID**
```bash
docker ps
# Look for the container running 'eclipse-mosquitto'
# Note down the CONTAINER ID (e.g., abc123def456)
```

### **3. Access the Container and Create Users**
```bash
# Login interactively into the mqtt container
docker exec -it <container-id> sh

# Create the password file and first user (beeper-service)
mosquitto_passwd -c /mosquitto/config/pwfile beeper-service
# It will prompt for password - use a strong password

# Add additional users (remove the -c option for subsequent users)
mosquitto_passwd /mosquitto/config/pwfile alerttx-device
# Prompt for AlertTX device password

mosquitto_passwd /mosquitto/config/pwfile admin  
# Prompt for admin password

# Exit the container
exit
```

### **4. Restart the Container**
```bash
docker restart <container-id>
```

### **5. Update Your Environment File**
```bash
cp env.template .env
nano .env

# Update with the passwords you just created:
MQTT_USERNAME=beeper-service
MQTT_PASSWORD=your-actual-password-here
```

## 🔧 **User Management Commands**

### **Add New User:**
```bash
docker exec -it <container-id> mosquitto_passwd /mosquitto/config/pwfile newuser
```

### **Delete User:**
```bash
docker exec -it <container-id> mosquitto_passwd -D /mosquitto/config/pwfile username
```

### **View Password File:**
```bash
docker exec -it <container-id> cat /mosquitto/config/pwfile
```

## 🧪 **Test the Setup**

### **Install MQTT Client Tools:**
```bash
# On Ubuntu/Debian:
sudo apt install mosquitto-clients

# On macOS:
brew install mosquitto
```

### **Test Subscription (AlertTX Device):**
```bash
mosquitto_sub -h localhost -t 'alerts/+/+' -u alerttx-device -P your-device-password
```

### **Test Publishing (Beeper Service):**
```bash
mosquitto_pub -h localhost -t 'alerts/high/triggered' -u beeper-service -P your-service-password -m '{"test": "message"}'
```

## 🔐 **Security Notes**

### **User Roles:**
- **`beeper-service`**: Can only publish to `alerts/+/+` topics
- **`alerttx-device`**: Can only subscribe to `alerts/+/+` topics  
- **`admin`**: Full access to all topics (for debugging)

### **Access Control:**
The ACL file restricts each user to their specific role:
```bash
# View current ACL:
docker exec -it <container-id> cat /mosquitto/config/acl
```

### **Password Security:**
- Use strong, unique passwords for each user
- Store passwords securely (password managers)
- Rotate passwords regularly
- Never commit actual passwords to git

## 🐛 **Troubleshooting**

### **Container Won't Start:**
```bash
# Check logs
docker logs <container-id>

# Common issues:
# - Permission errors on mounted files
# - Invalid mosquitto.conf syntax
```

### **Authentication Fails:**
```bash
# Verify user exists
docker exec -it <container-id> cat /mosquitto/config/pwfile

# Check mosquitto logs
docker logs <container-id> -f
```

### **Connection Refused:**
```bash
# Verify container is running and ports are exposed
docker ps
netstat -tlnp | grep 1883
```

## 📊 **Monitoring**

### **Check Connected Clients:**
```bash
mosquitto_sub -h localhost -t '$SYS/broker/clients/connected' -u admin -P your-admin-password
```

### **Monitor All Traffic (Admin Only):**
```bash
mosquitto_sub -h localhost -t '#' -u admin -P your-admin-password
```

This approach gives you full control over password management while keeping the setup simple and secure! 🔒
