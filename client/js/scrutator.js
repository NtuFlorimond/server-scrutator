const evtSource = new EventSource("/scrutator");

evtSource.addEventListener("file-added", e => {
  const data = JSON.parse(e.data);
  console.log("Nouveau fichier :", data.file);
  // Rafraîchir seulement la section souhaitée
  // ou location.reload();
});

evtSource.onerror = e => {
  console.error("Erreur SSE :", e);
};
