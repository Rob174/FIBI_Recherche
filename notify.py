import argparse
import requests

# Define your Pushbullet API key
API_KEY = "o.HgcerfQeeymoj8vNPwAQaW36tZgqDEi7"

# Function to send a Pushbullet notification
def send_notification(status, message):
    title = f"Script {status}"
    body = f"{status.lower()}.\nMessage: {message}"
    
    data = {
        "type": "note",
        "title": title,
        "body": body
    }
    
    response = requests.post(
        "https://api.pushbullet.com/v2/pushes",
        data=data,
        headers={"Authorization": "Bearer " + API_KEY}
    )
    
    if response.status_code != 200:
        print("Failed to send notification:", response.text)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--status", help="Status of the script")
    parser.add_argument("--message", help="Additional message")
    args = parser.parse_args()
    
    send_notification(args.status, args.message)