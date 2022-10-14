window.addEventListener('load', function() {
    document.querySelectorAll('a.reference.external').forEach(function(link) {
        link.target = '_blank';
    });

    document.querySelectorAll('.footer a').forEach(function(link) {
        link.target = '_blank';
    });

    document.querySelectorAll('form input[type="text"]').forEach(function(input) {
        input.setAttribute("placeholder", "Search docs");
    });
});