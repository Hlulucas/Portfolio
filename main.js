// Visionneuse pour les captures + bouton "copier" de l'adresse mail.

// Visionneuse : sans JS, les liens ouvrent simplement l'image.
const liens = [...document.querySelectorAll("a[data-lightbox]")];

if (liens.length && typeof HTMLDialogElement === "function") {
    const boite = document.createElement("dialog");
    boite.className = "lightbox";
    boite.innerHTML = `
        <img alt="">
        <div class="lightbox-bar">
            <p></p>
            <div>
                <button type="button" data-pas="-1">Précédente</button>
                <button type="button" data-pas="1">Suivante</button>
                <button type="button" data-fermer>Fermer</button>
            </div>
        </div>`;
    document.body.append(boite);

    const image = boite.querySelector("img");
    const legende = boite.querySelector("p");
    let actuelle = 0;

    function afficher(i) {
        actuelle = (i + liens.length) % liens.length;
        const lien = liens[actuelle];
        image.src = lien.href;
        image.alt = lien.querySelector("img")?.alt ?? "";
        legende.textContent = `${lien.textContent.trim()} (${actuelle + 1}/${liens.length})`;
    }

    liens.forEach((lien, i) => {
        lien.addEventListener("click", (e) => {
            e.preventDefault();
            afficher(i);
            boite.showModal();
        });
    });

    boite.addEventListener("click", (e) => {
        if (e.target === boite || e.target.closest("[data-fermer]")) boite.close();
        const bouton = e.target.closest("[data-pas]");
        if (bouton) afficher(actuelle + Number(bouton.dataset.pas));
    });

    boite.addEventListener("keydown", (e) => {
        if (e.key === "ArrowRight") afficher(actuelle + 1);
        if (e.key === "ArrowLeft") afficher(actuelle - 1);
    });
}

// Copier l'adresse mail
document.querySelectorAll("[data-copier]").forEach((bouton) => {
    bouton.addEventListener("click", async () => {
        try {
            await navigator.clipboard.writeText(bouton.dataset.copier);
            bouton.textContent = "Copiée !";
        } catch {
            window.location.href = "mailto:" + bouton.dataset.copier;
            return;
        }
        setTimeout(() => (bouton.textContent = "Copier"), 2000);
    });
});
