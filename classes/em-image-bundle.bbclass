# em-image-bundle.bbclass: convenience class for image+bundle pairs
#
# Inheriting em-image-bundle from a recipe PN will create three recipes:
# a product-info package em-product-info-PN, an image em-image-PN and a RAUC
# bundle em-bundle-PN which includes the image.



BBCLASSEXTEND = "em-product-info:product-info em-image:image em-bundle:bundle"

python em_image_bundle_virtclass_handler () {
    variant = e.data.getVar("BBEXTENDVARIANT")
    if variant is None:
        raise bb.parse.SkipPackage("Unextended em-image-bundle")

    e.data.setVar('PN', 'em-%s-%s' % (variant, e.data.getVar('PN', False)))
}

addhandler em_image_bundle_virtclass_handler
em_image_bundle_virtclass_handler[eventmask] = "bb.event.RecipePreFinalise"
