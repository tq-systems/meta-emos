# em-image-bundle.bbclass: convenience class for image+bundle pairs
#
# Inheriting em-image-bundle from a recipe PN will create three recipes:
# a product-info package em-product-info-PN, an image em-image-PN and a RAUC
# bundle em-bundle-PN which includes the image.



BBCLASSEXTEND = "em-product-info:product-info em-image:image em-bundle:bundle"

python em_image_bundle_virtclass_handler () {
    recipe_type = e.data.getVar("BBEXTENDVARIANT")
    if recipe_type is None:
        raise bb.parse.SkipPackage("Unextended em-image-bundle")

    image_variant = e.data.getVar('PN', False)

    e.data.setVar('IMAGE_VARIANT', image_variant)
    e.data.setVar('PN', 'em-%s-%s' % (recipe_type, image_variant))
}

addhandler em_image_bundle_virtclass_handler
em_image_bundle_virtclass_handler[eventmask] = "bb.event.RecipePreFinalise"

IMAGE_VARIANT_COMPATIBLE ?= "${IMAGE_VARIANT}"
IMAGE_COMPATIBLE = "${MACHINE}/${IMAGE_VARIANT_COMPATIBLE}"

PRODUCT_INFO_PACKAGE ?= "em-product-info-${IMAGE_VARIANT}"
