#!/bin/bash

# Arrêter et supprimer le conteneur s'il existe
docker stop cImage 
docker rm cImage 

# Télécharger l'image Docker si nécessaire et démarrer le conteneur avec imagick
docker pull sae103-imagick
docker container run -di --name cImage sae103-imagick

# Initialisation de variables
nombre=0
depart_doss="../Etape_1"
arriver_doss="../Etape_1/images_converties"

# Création du répertoire de sortie
mkdir -p "$arriver_doss"

# Parcourir les fichiers dans le répertoire depatr
for fichier in "$depart_doss"/*.{webp,jpg,png,pdf}; do
    if [ -f "$fichier" ]; then
        # Copier le fichier vers le conteneur
        docker cp "$fichier" cImage:/data/
        
        # Conversion du fichier en .webp avec comme nom des numéros
        docker container exec cImage convert /data/"$(basename "$fichier")" /data/"$nombre.webp"
        
        if [ $? -eq 0 ]; then
            # Redimensionner l'image convertie
            docker container exec cImage convert /data/"$nombre.webp" -resize 900x600 /data/"$nombre.webp"
            
            # Récupérer le fichier redimensionné
            docker cp cImage:/data/"$nombre.webp" "$arriver_doss/"
        else
            echo "Erreur : la conversion du fichier $fichier a échoué."
        fi
        
        # Incrémentation du compteur pour le nom de chaque image
        nombre=$((nombre + 1))
    fi
done

# Arrêter et supprimer le conteneur
docker stop cImage
docker rm cImage

echo "Traitement terminé. Les fichiers convertis sont dans $arriver_doss."

