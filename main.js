/* Lucas Hurtrel — Portfolio : interactions (JavaScript natif, sans dépendance) */
(function () {
    "use strict";

    var reduceMotion = window.matchMedia("(prefers-reduced-motion: reduce)").matches;

    /* ---------- En-tête : bordure au défilement ---------- */
    var header = document.querySelector(".site-header");
    function onScroll() {
        if (header) header.classList.toggle("is-scrolled", window.scrollY > 8);
    }
    onScroll();
    window.addEventListener("scroll", onScroll, { passive: true });

    /* ---------- Menu mobile ---------- */
    var toggle = document.querySelector(".nav-toggle");
    var nav = document.getElementById("site-nav");
    if (toggle && nav) {
        var closeNav = function () {
            nav.classList.remove("is-open");
            toggle.setAttribute("aria-expanded", "false");
            toggle.setAttribute("aria-label", "Ouvrir le menu");
        };
        toggle.addEventListener("click", function () {
            var open = nav.classList.toggle("is-open");
            toggle.setAttribute("aria-expanded", String(open));
            toggle.setAttribute("aria-label", open ? "Fermer le menu" : "Ouvrir le menu");
        });
        nav.addEventListener("click", function (e) {
            if (e.target.closest("a")) closeNav();
        });
        document.addEventListener("keydown", function (e) {
            if (e.key === "Escape") closeNav();
        });
        window.addEventListener("resize", function () {
            if (window.innerWidth > 760) closeNav();
        });
    }

    /* ---------- Apparition au défilement ---------- */
    var revealItems = document.querySelectorAll(".reveal");
    if ("IntersectionObserver" in window && !reduceMotion) {
        var io = new IntersectionObserver(function (entries) {
            entries.forEach(function (entry) {
                if (entry.isIntersecting) {
                    entry.target.classList.add("is-visible");
                    io.unobserve(entry.target);
                }
            });
        }, { rootMargin: "0px 0px -6% 0px", threshold: 0.06 });
        revealItems.forEach(function (el) { io.observe(el); });
    } else {
        revealItems.forEach(function (el) { el.classList.add("is-visible"); });
    }

    /* ---------- Effet machine à écrire ---------- */
    var typer = document.querySelector("[data-typer]");
    if (typer && !reduceMotion) {
        var words = [];
        try { words = JSON.parse(typer.getAttribute("data-typer")); } catch (err) { words = []; }
        if (words.length > 1) {
            var w = 0;
            var i = words[0].length;
            var deleting = true;
            var tick = function () {
                var word = words[w];
                if (deleting) {
                    i -= 1;
                    typer.textContent = word.slice(0, i);
                    if (i <= 0) {
                        deleting = false;
                        w = (w + 1) % words.length;
                        setTimeout(tick, 350);
                    } else {
                        setTimeout(tick, 32);
                    }
                } else {
                    i += 1;
                    typer.textContent = word.slice(0, i);
                    if (i >= word.length) {
                        deleting = true;
                        setTimeout(tick, 2200);
                    } else {
                        setTimeout(tick, 62);
                    }
                }
            };
            setTimeout(tick, 2400);
        }
    }

    /* ---------- Filtres des projets ---------- */
    var filterBar = document.querySelector("[data-filters]");
    if (filterBar) {
        var buttons = filterBar.querySelectorAll("button[data-filter]");
        var blocks = document.querySelectorAll("[data-cat]");
        buttons.forEach(function (btn) {
            btn.addEventListener("click", function () {
                var f = btn.getAttribute("data-filter");
                buttons.forEach(function (b) { b.setAttribute("aria-pressed", String(b === btn)); });
                blocks.forEach(function (el) {
                    var show = f === "all" || el.getAttribute("data-cat") === f;
                    el.hidden = !show;
                    if (show) el.classList.add("is-visible");
                });
            });
        });
    }

    /* ---------- Visionneuse d'images ---------- */
    var shots = Array.prototype.slice.call(document.querySelectorAll("[data-lightbox]"));
    if (shots.length && typeof HTMLDialogElement === "function") {
        var dlg = document.createElement("dialog");
        dlg.className = "lightbox";
        dlg.setAttribute("aria-label", "Visionneuse d'images");
        dlg.innerHTML =
            '<img alt="">' +
            '<div class="lightbox-bar">' +
            '<p class="lightbox-caption"></p>' +
            '<div class="lightbox-controls">' +
            '<button type="button" class="icon-btn" data-dir="-1" aria-label="Image précédente">&larr;</button>' +
            '<button type="button" class="icon-btn" data-dir="1" aria-label="Image suivante">&rarr;</button>' +
            '<button type="button" class="icon-btn" data-close aria-label="Fermer">&times;</button>' +
            "</div></div>";
        document.body.appendChild(dlg);

        var lbImg = dlg.querySelector("img");
        var lbCap = dlg.querySelector(".lightbox-caption");
        var current = 0;

        var show = function (index) {
            current = (index + shots.length) % shots.length;
            var link = shots[current];
            var thumb = link.querySelector("img");
            lbImg.src = link.getAttribute("href");
            lbImg.alt = thumb ? thumb.alt : "";
            lbCap.textContent = (current + 1) + " / " + shots.length + " · " + (link.getAttribute("data-caption") || "");
        };

        shots.forEach(function (link, index) {
            link.addEventListener("click", function (e) {
                e.preventDefault();
                show(index);
                dlg.showModal();
            });
        });

        dlg.addEventListener("click", function (e) {
            if (e.target === dlg || e.target.closest("[data-close]")) {
                dlg.close();
                return;
            }
            var dirBtn = e.target.closest("[data-dir]");
            if (dirBtn) show(current + Number(dirBtn.getAttribute("data-dir")));
        });

        dlg.addEventListener("keydown", function (e) {
            if (e.key === "ArrowRight") show(current + 1);
            if (e.key === "ArrowLeft") show(current - 1);
        });
    }

    /* ---------- Copier l'adresse mail ---------- */
    var toastEl = null;
    var toastTimer = null;
    function showToast(message) {
        if (!toastEl) {
            toastEl = document.createElement("div");
            toastEl.className = "toast";
            toastEl.setAttribute("role", "status");
            toastEl.setAttribute("aria-live", "polite");
            document.body.appendChild(toastEl);
        }
        toastEl.textContent = message;
        toastEl.classList.add("is-visible");
        clearTimeout(toastTimer);
        toastTimer = setTimeout(function () { toastEl.classList.remove("is-visible"); }, 2200);
    }

    document.querySelectorAll("[data-copy]").forEach(function (btn) {
        btn.addEventListener("click", function () {
            var text = btn.getAttribute("data-copy");
            if (navigator.clipboard && window.isSecureContext) {
                navigator.clipboard.writeText(text).then(
                    function () { showToast("Adresse copiée dans le presse-papiers"); },
                    function () { window.location.href = "mailto:" + text; }
                );
            } else {
                window.location.href = "mailto:" + text;
            }
        });
    });

    /* ---------- Année du pied de page ---------- */
    document.querySelectorAll("[data-year]").forEach(function (el) {
        el.textContent = String(new Date().getFullYear());
    });
})();
