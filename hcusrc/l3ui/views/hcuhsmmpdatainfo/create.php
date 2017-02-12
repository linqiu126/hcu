<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcuhsmmpdatainfo */

$this->title = 'Create Hcuhsmmpdatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcuhsmmpdatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuhsmmpdatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
