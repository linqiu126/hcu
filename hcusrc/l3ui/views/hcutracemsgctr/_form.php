<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcutracemsgctr */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcutracemsgctr-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'msgid')->textInput() ?>

    <?= $form->field($model, 'msgname')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'msgctrflag')->textInput() ?>

    <?= $form->field($model, 'msgallow')->textInput() ?>

    <?= $form->field($model, 'msgrestrict')->textInput() ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
