
import pyrebase

#Se pone la información de la base de datos
config = {
"apiKey": "AIzaSyBdwl6PBpsNMa-6uVwxKVOUqrO2cYlhaEI",
  "authDomain": "proyectoesp32-91402.firebaseapp.com",
  "databaseURL": "https://proyectoesp32-91402-default-rtdb.firebaseio.com",
  "projectId": "proyectoesp32-91402",
  "storageBucket": "proyectoesp32-91402.appspot.com",
  "messagingSenderId": "1093219874621",
  "appId": "1:1093219874621:web:bc0d0b1400d5f74f5db754",
  "measurementId": "G-FWQXVQTG78"
}
#crea la autenticacioon
firebase = pyrebase.initialize_app(config)
#se accesa a la base de datos en Firebase
db = firebase.database()
#Escribe datos
data = {"name":"Abraham"}
data1 = {"name":"Emilio"}
data2 = {"name" : "Francisco"}
db.child("creadores").push(data)
db.child("creadores").push(data1)
db.child("creadores").push(data2)
print("Data added to real time database ")

#Lee todos los datos de test
all_users = db.child("test").get()
for users in all_users.each():
    print(users.key(), " : " , users.val())
    #Guarda el valor de numero en una variable
    if(users.key() == "numero"):
        numero = users.val()
print("Se han impreso todos los datos de los sensores")

#Si es mayor a 9 cambia el usuario como mayor a nueve
if(int(numero) > 9):
    db.child("test").update({"usuario": "elemento mayor a 9"})
    print("Se ha cambiado la variable usuario. ")
else:
    print("No se cambio porque el numero es menor a 9")
