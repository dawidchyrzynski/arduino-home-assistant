window.addEventListener('load', function() {
    document.querySelectorAll('a.reference.external').forEach(function(link) {
        link.target = '_blank';
    });
});