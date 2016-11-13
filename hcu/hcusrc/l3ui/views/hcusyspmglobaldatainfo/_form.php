<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcusyspmglobaldatainfo */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcusyspmglobaldatainfo-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'taskhcuvmec')->textInput() ?>

    <?= $form->field($model, 'taskhwinvec')->textInput() ?>

    <?= $form->field($model, 'tasksyspmec')->textInput() ?>

    <?= $form->field($model, 'taskmodbusec')->textInput() ?>

    <?= $form->field($model, 'taskcloudvelaec')->textInput() ?>

    <?= $form->field($model, 'taskavorionec')->textInput() ?>

    <?= $form->field($model, 'taskspsvirgoec')->textInput() ?>

    <?= $form->field($model, 'taskhsmmpec')->textInput() ?>

    <?= $form->field($model, 'taskemcec')->textInput() ?>

    <?= $form->field($model, 'taskpm25ec')->textInput() ?>

    <?= $form->field($model, 'taskwinddirec')->textInput() ?>

    <?= $form->field($model, 'taskwindspdec')->textInput() ?>

    <?= $form->field($model, 'tasktempec')->textInput() ?>

    <?= $form->field($model, 'taskhumidec')->textInput() ?>

    <?= $form->field($model, 'tasknoiseec')->textInput() ?>

    <?= $form->field($model, 'taskairprsec')->textInput() ?>

    <?= $form->field($model, 'taskco1ec')->textInput() ?>

    <?= $form->field($model, 'tasklightstrec')->textInput() ?>

    <?= $form->field($model, 'taskalcoholec')->textInput() ?>

    <?= $form->field($model, 'taskhchoec')->textInput() ?>

    <?= $form->field($model, 'tasktoxicgasec')->textInput() ?>

    <?= $form->field($model, 'restartcnt')->textInput() ?>

    <?= $form->field($model, 'cloudvelaconncnt')->textInput() ?>

    <?= $form->field($model, 'cloudveladisccnt')->textInput() ?>

    <?= $form->field($model, 'clouddatatimeoutcnt')->textInput() ?>

    <?= $form->field($model, 'timestamp')->textInput() ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
