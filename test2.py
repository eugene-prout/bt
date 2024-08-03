import requests

info_hash = r"%2a%a4%f5%a7%e2%09%e5%4b%32%80%3d%43%67%09%71%c4%c8%ca%aa%05"
peer_id = bytes("-TR2940-k8hj0wgej6ch", 'utf-8').decode('latin-1')
port = '6881'
uploaded = '0'
downloaded = '0'
left = str(12345)
compact = '1'

url = f"https://torrent.ubuntu.com/announce?info_hash={info_hash}&peer_id={peer_id}&port={port}&uploaded={uploaded}&downloaded={downloaded}&left={left}&compact={compact}"

print(requests.get(url).text)